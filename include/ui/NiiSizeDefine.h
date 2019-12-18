/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-8-20

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

#ifndef _NII_UI_SIZEDEFINE_H_
#define _NII_UI_SIZEDEFINE_H_

#include "NiiUIPreInclude.h"
#include "NiiUICommon.h"
#include "NiiRelPos.h"

namespace NII
{
namespace UI
{
    /** 大小基础类
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SizeBase : public UIAlloc
    {
    public:
        SizeBase();
        SizeBase(SizeType type);
        virtual ~SizeBase();

        /** 设置大小的类型
        @version NIIEngine 3.0.0
        */
        inline void setType(SizeType type) { mType = type; }

        /** 获取大小的类型
        @version NIIEngine 3.0.0
        */
        inline SizeType getType() const { return mType; }

        /** 获取大小
        @param wid 大小的来源
        @version NIIEngine 3.0.0
        */
        virtual NIIf getValue(const Widget & wid) const = 0;

        /** 获取大小
        @param wid 大小的来源
        @param container 大小裁剪
        @version NIIEngine 3.0.0
        */
        virtual NIIf getValue(const Widget & wid, const Rectf & container) const = 0;

        /** 对象副本
        @version NIIEngine 3.0.0
        */
        virtual SizeBase * clone() const = 0;

        /** 写入到XML
        @version NIIEngine 3.0.0
        */
        virtual void write(XmlSerializer * out) const;
    protected:
        /** 写入标签
        @version NIIEngine 3.0.0
        */
        virtual void writeTag(XmlSerializer * out) const = 0;

        /** 写入内容
        @version NIIEngine 3.0.0
        */
        virtual void writeValue(XmlSerializer * out) const = 0;
    protected:
        SizeType mType;
    };

    /** 大小定义
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SizeDefine : public UIAlloc
    {
    public:
        SizeDefine();
        SizeDefine(const SizeDefine & o);
        SizeDefine(const SizeBase & value, SizeType type);
        ~SizeDefine();
        SizeDefine & operator=(const SizeDefine & o);

        /** 设置值
        @version NIIEngine 3.0.0
        */
        void setValue(SizeBase * value, bool copy);

        /** 获取值
        @version NIIEngine 3.0.0
        */
        inline const SizeBase * getValue() const { return mBase; }

        /** 设置值类型
        @version NIIEngine 3.0.0
        */
        inline void setType(SizeType type) { mType = type; };

        /** 获取值类型
        @version NIIEngine 3.0.0
        */
        inline SizeType getType() const { return mType; }

        /** 输出到XML序列
        @param out
        @version NIIEngine 3.0.0
        */
        void write(XmlSerializer * out) const;
    private:
        SizeType mType;
        SizeBase * mBase;
    };
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RelationSize : public SizeBase
    {
    public:
        RelationSize() {};
        RelationSize(const RelPos & value, SizeType type);

        /// 设置值
        void setBaseValue(const RelPos & val);

        /// 获取值
        const RelPos & getBaseValue() const;

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & w) const;

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & w, const Rectf & container) const;

        /// @copydetails SizeBase::clone
        SizeBase * clone() const;
    protected:
        /// @copydetails SizeBase::writeTag
        void writeTag(XmlSerializer * out) const;

        /// @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * out) const;
    private:
        RelPos mValue;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AbsoluteSize : public SizeBase
    {
    public:
        AbsoluteSize();
        AbsoluteSize(NIIf val);

        /// 设置值
        void setBaseValue(NIIf val);

        /// 获取值
        NIIf getBaseValue() const;

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & w) const;

        /// @copydetails SizeBase::getValue
        NIIf getValue(const Widget & w, const Rectf & container) const;

        /// @copydetails SizeBase::clone
        SizeBase * clone() const;
    protected:
        /// @copydetails SizeBase::writeTag
        void writeTag(XmlSerializer * out) const;

        /// @copydetails SizeBase::writeValue
        void writeValue(XmlSerializer * out) const;
    private:
        NIIf mValue;
    };
}
}
#endif