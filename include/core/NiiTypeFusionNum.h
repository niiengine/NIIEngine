/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-6-21

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
            out = C::conv(mObj);
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
    typedef TypeFusionNum<StrConv, Colour>	 ColourFusion;
}
#endif