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

#ifndef _NII_FUSION_NUM_H_
#define _NII_FUSION_NUM_H_

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
}
#endif