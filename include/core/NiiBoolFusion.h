/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-12

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
三维应用软件
计算机辅助立体设计软件(CAD)
地理信息系统软件(GIS)
电影背景立体重构软件
立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_BOOL_FUSION_H_
#define _NII_BOOL_FUSION_H_

#include "NiiPreInclude.h"
#include "NiiFusionNum.h"
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
            out = StrConv::conv(mObj);
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