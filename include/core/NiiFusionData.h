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