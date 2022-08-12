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

#ifndef _NII_PropertyType_H_
#define _NII_PropertyType_H_

#include "NiiPreInclude.h"
#include "NiiPropertyObj.h"
#include "NiiXmlSerializer.h"
#include "NiiUICommon.h"

using namespace NII::UI;

namespace NII
{
    /** 属性类型
    @note UIStrConv 需要实现具体的类型到字符串之间的转换
    @version NIIEngine 3.0.0
    */
    template<typename T> class PropertyType
    {
    public:
        PropertyType() :
            mValue(fromStr(_T(""))){}

        PropertyType(PropertyID pid) :
            mValue(fromStr(_T(""))),
            mPropertyID(pid){}

        PropertyType(T val) :
            mValue(val){}

        inline bool operator==(const PropertyType<T> & o) const
        {
            return mValue == o.mValue && mPropertyID == o.mPropertyID;
        }

        inline bool operator!=(const PropertyType<T> & o) const
        {
            return mValue != o.mValue || mPropertyID != o.mPropertyID;
        }

        T get(const PropertyCmdObj * obj) const
        {
            if(mPropertyID == 0)
                return mValue;

            T temp;
            obj->get(mPropertyID, &temp);
            return temp;
        }

        void set(T val)
        {
            mValue = val;
            mPropertyID = 0;
        }

        inline void setSrcFetch(PropertyID pid)
        {
            mPropertyID = pid;
        }

        inline bool isSrcFetch() const
        {
            return mPropertyID != 0;
        }

        inline String toStr() const
        {
            return UIStrConv::conv(mValue);
        }

        inline T fromStr(const String & str)
        {
            UIStrConv::conv(str, mValue);
            return mValue;
        }

        void write(XmlSerializer * out) const
        {
            writeTag(out);
            writeValue(out);
            out->end();
        }

        virtual void writeTag(XmlSerializer * out) const
        {
            (void)out;
        }

        virtual void writeValue(XmlSerializer * out) const
        {
            if (mPropertyID == 0)
                out->attribute(_T("type"), N_conv(mValue));
            else
                out->attribute(_T("name"), mPropertyID);
        }
    protected:
        T mValue;
        PropertyID mPropertyID;
    };

    typedef PropertyType<HAlign> HAlignPropertyType;
    typedef PropertyType<VAlign> VAlignPropertyType;
    typedef PropertyType<VLayout> VLayoutPropertyType;
    typedef PropertyType<HLayout> HLayoutPropertyType;
    typedef PropertyType<HTextLayout> HTextLayoutPropertyType;
    typedef PropertyType<VTextLayout> VTextLayoutPropertyType;
    template<> void PropertyType<HTextLayout>::writeTag(XmlSerializer * out) const
    {
        if (mPropertyID == 0)
            out->begin(_T("HLayout"));
        else
            out->begin(_T("HLayoutProperty"));
    }
    template<> void PropertyType<HLayout>::writeTag(XmlSerializer * out) const
    {
        if (mPropertyID == 0)
            out->begin(_T("HLayout"));
        else
            out->begin(_T("HLayoutProperty"));
    }
    template<> void PropertyType<VLayout>::writeTag(XmlSerializer * out) const
    {
        if (mPropertyID == 0)
            out->begin(_T("VLayout"));
        else
            out->begin(_T("VLayoutProperty"));
    }
    template<> void PropertyType<VTextLayout>::writeTag(XmlSerializer * out) const
    {
        if (mPropertyID == 0)
            out->begin(_T("VLayout"));
        else
            out->begin(_T("VLayoutProperty"));
    }
}
#endif