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

#ifndef _NII_UI_SIZEDEFINE_H_
#define _NII_UI_SIZEDEFINE_H_

#include "NiiUIPreInclude.h"
#include "NiiUICommon.h"
#include "NiiPlaneSize.h"

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
        RelationSize(const RelPosf & value, SizeType type);

        /// 设置值
        void setBaseValue(const RelPosf & val);

        /// 获取值
        const RelPosf & getBaseValue() const;

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
        RelPosf mValue;
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