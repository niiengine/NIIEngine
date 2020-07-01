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
        FusionNum * getBaseValue() const;

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
        void setAutoDestroy(bool b);

        /** 获取是否自动删除混合数据
        @version NIIEngine 3.0.0
        */
        bool isAutoDestroy() const;

        /** 同类型加法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator +=(const FusionData & o);

        /** 同类型减法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator -=(const FusionData & o);

        /** 同类型乘法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator *=(const FusionData & o);

        /** 同类型除法
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator /=(const FusionData & o);

        /** 值偏移(加法)
        @version NIIEngine 3.0.0
        */
        FusionData & operator +=(NIIf o);

        /** 值偏移(减法)
        @version NIIEngine 3.0.0
        */
        FusionData & operator -=(NIIf o);

        /** 值偏移(乘法)
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator *=(NIIf o);

        /** 值偏移(除法)
        @remark 必须是同类型的,否则会异常
        @version NIIEngine 3.0.0
        */
        FusionData & operator /=(NIIf o);

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