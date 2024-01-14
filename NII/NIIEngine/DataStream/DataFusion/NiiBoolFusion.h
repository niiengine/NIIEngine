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

#ifndef _NII_BOOL_FUSION_H_
#define _NII_BOOL_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiFusionData.h"
#include "NiiStrConv.h"
#include "NiiException.h"

namespace NII
{
    /** 隐式基础类型混算
    @version NIIEngine 3.0.0
    */
    class _EngineAPI BoolFusion : public FusionNum
    {
    public:
        BoolFusion(){};
        BoolFusion(const bool & value) : mObj(value){}
        BoolFusion(const FusionNum & obj)
        {
            mObj = static_cast<const BoolFusion &>(obj).mObj;
        }

        /// @copydetails FusionNum::Add
        void Add(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<BoolFusion *>(out)->mObj =
                mObj || static_cast<const BoolFusion *>(in)->mObj;
        }

        /// @copydetails FusionNum::Sub
        void Sub(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<BoolFusion *>(out)->mObj =
                mObj && !static_cast<const BoolFusion *>(in)->mObj;
        }

        /// @copydetails FusionNum::Mul
        void Mul(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            static_cast<BoolFusion *>(out)->mObj =
                mObj && static_cast<const BoolFusion *>(in)->mObj;
        }

        /// @copydetails FusionNum::Div
        void Div(const FusionNum * in, FusionNum * out)
        {
            N_assert(in, "error");
            N_assert(out, "error");
            if(static_cast<const BoolFusion *>(in)->mObj)
                static_cast<BoolFusion *>(out)->mObj = mObj;
            else
                static_cast<BoolFusion *>(out)->mObj = false;
        }

        /// @copydetails FusionNum::Add
        void Add(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            if(in || mObj)
                static_cast<BoolFusion *>(out)->mObj = true;
            else
                static_cast<BoolFusion *>(out)->mObj = false;
        }

        /// @copydetails FusionNum::Sub
        void Sub(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            if(!in && mObj)
                static_cast<BoolFusion *>(out)->mObj = true;
            else
                static_cast<BoolFusion *>(out)->mObj = false;
        }

        /// @copydetails FusionNum::Mul
        void Mul(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            static_cast<BoolFusion *>(out)->mObj = mObj && in;
        }

        /// @copydetails FusionNum::Div
        void Div(NIIf in, FusionNum * out)
        {
            N_assert(out, "error");
            if(in)
                static_cast<BoolFusion *>(out)->mObj = mObj;
            else
                static_cast<BoolFusion *>(out)->mObj = false;
        }

        /// @copydetails FusionNum::Fusion
        virtual void Fusion(FusionNum * k1, FusionNum * k2, NIIf t)
        {
            if(t < 0.5)
                mObj = static_cast<BoolFusion *>(k1)->mObj;
            else
                mObj = static_cast<BoolFusion *>(k2)->mObj;
        }

        /// @copydetails FusionNum::RelFusion
        virtual void RelFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t)
        {
            if(t < 0.5)
                mObj = static_cast<BoolFusion *>(k1)->mObj || static_cast<BoolFusion *>(bas)->mObj;
            else
                mObj = static_cast<BoolFusion *>(k2)->mObj || static_cast<BoolFusion *>(bas)->mObj;
        }

        /// @copydetails FusionNum::RelMulFusion
        virtual void RelMulFusion(FusionNum * bas, FusionNum * k1, FusionNum * k2, NIIf t)
        {
            if(t < 0.5)
                mObj = static_cast<BoolFusion *>(k1)->mObj && static_cast<BoolFusion *>(bas)->mObj;
            else
                mObj = static_cast<BoolFusion *>(k2)->mObj && static_cast<BoolFusion *>(bas)->mObj;
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
        void setData(const bool & data)
        {
            mObj = data;
        }

        /** 获取值
        @version NIIEngine 3.0.0
        */
        const bool & getData() const
        {
            return mObj;
        }

        /// @copydetails FusionNum::setData
        void setData(const void * data)
        {
            mObj = *(bool *)data;
        }

        /// @copydetails FusionNum::getDataPtr
        const void * getDataPtr() const
        {
            return &mObj;
        }

        /// @copydetails FusionNum::cloneType
        FusionNum * cloneType() const
        {
            return N_new BoolFusion();
        }

        /// @copydetails FusionNum::clone
        FusionNum * clone() const
        {
            return N_new BoolFusion(mObj);
        }
    protected:
        bool mObj;
    };
}
#endif