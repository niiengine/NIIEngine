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
    /** ��С������
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SizeBase : public UIAlloc
    {
    public:
        SizeBase();
        SizeBase(SizeType type);
        virtual ~SizeBase();

        /** ���ô�С������
        @version NIIEngine 3.0.0
        */
        inline void setType(SizeType type) { mType = type; }

        /** ��ȡ��С������
        @version NIIEngine 3.0.0
        */
        inline SizeType getType() const { return mType; }

        /** ��ȡ��С
        @param wid ��С����Դ
        @version NIIEngine 3.0.0
        */
        virtual NIIf getValue(const Widget & wid) const = 0;

        /** ��ȡ��С
        @param wid ��С����Դ
        @param container ��С�ü�
        @version NIIEngine 3.0.0
        */
        virtual NIIf getValue(const Widget & wid, const Rectf & container) const = 0;

        /** ���󸱱�
        @version NIIEngine 3.0.0
        */
        virtual SizeBase * clone() const = 0;

        /** д�뵽XML
        @version NIIEngine 3.0.0
        */
        virtual void write(XmlSerializer * out) const;
    protected:
        /** д���ǩ
        @version NIIEngine 3.0.0
        */
        virtual void writeTag(XmlSerializer * out) const = 0;

        /** д������
        @version NIIEngine 3.0.0
        */
        virtual void writeValue(XmlSerializer * out) const = 0;
    protected:
        SizeType mType;
    };

    /** ��С����
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

        /** ����ֵ
        @version NIIEngine 3.0.0
        */
        void setValue(SizeBase * value, bool copy);

        /** ��ȡֵ
        @version NIIEngine 3.0.0
        */
        inline const SizeBase * getValue() const { return mBase; }

        /** ����ֵ����
        @version NIIEngine 3.0.0
        */
        inline void setType(SizeType type) { mType = type; };

        /** ��ȡֵ����
        @version NIIEngine 3.0.0
        */
        inline SizeType getType() const { return mType; }

        /** �����XML����
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

        /// ����ֵ
        void setBaseValue(const RelPosf & val);

        /// ��ȡֵ
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

        /// ����ֵ
        void setBaseValue(NIIf val);

        /// ��ȡֵ
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