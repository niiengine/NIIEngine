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

#include "NiiPreProcess.h"
#include "NiiSizeDefine.h"
#include "NiiPixelManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIException.h"
#include "NiiUIStrConv.h"
#include "NiiUIWidget.h"
#include "NiiXmlSerializer.h"

namespace NII
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SizeBase
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SizeBase::SizeBase()
    {
    }
    //------------------------------------------------------------------------
    SizeBase::SizeBase(SizeType type) : mType(type)
    {
    }
    //------------------------------------------------------------------------
    SizeBase::~SizeBase()
    {
    }
    //------------------------------------------------------------------------
    void SizeBase::write(XmlSerializer * out) const
    {
        // get sub-class to output the data for this single size
        writeTag(out);
        writeValue(out);
        out->end();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // SizeDefine
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    SizeDefine::SizeDefine():
        mBase(0),
        mType(ST_Unknow)
    {
    }
    //------------------------------------------------------------------------
    SizeDefine::~SizeDefine()
    {
        if(mBase)
            N_delete mBase;
    }
    //------------------------------------------------------------------------
    SizeDefine::SizeDefine(const SizeBase & dim, SizeType type)
    {
        mBase = dim.clone();
        mType = type;
    }
    //------------------------------------------------------------------------
    SizeDefine::SizeDefine(const SizeDefine & o)
    {
        mBase = o.mBase ? o.mBase->clone() : 0;
        mType = o.mType;
    }
    //------------------------------------------------------------------------
    SizeDefine & SizeDefine::operator=(const SizeDefine & o)
    {
        if(mBase)
            N_delete mBase;

        mBase = o.mBase ? o.mBase->clone() : 0;
        mType = o.mType;

        return *this;
    }
    //------------------------------------------------------------------------
    void SizeDefine::setValue(SizeBase * value, bool copy)
    {
        if(mBase)
            N_delete mBase;

        if (copy)
            mBase = value->clone();
        else
            mBase = value;
    }
    //------------------------------------------------------------------------
    void SizeDefine::write(XmlSerializer * out) const
    {
        out->begin(_T("Border")).attribute(_T("type"), NUI_conv(mType));

        if(mBase)
            mBase->write(out);
        out->end();
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RelationSize
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    RelationSize::RelationSize(const RelPosf & value, SizeType type) :
        SizeBase(type),
        mValue(value)
    {
    }
    //------------------------------------------------------------------------
    NIIf RelationSize::getValue(const Widget & wnd) const
    {
        switch (mType)
        {
        case ST_LeftPadding:
        case ST_RightPadding:
        case ST_X:
        case ST_XOft:
        case ST_Width:
            return mValue.abs(wnd.getPixelSize().mWidth);
            break;
        case ST_TopPadding:
        case ST_BottomPadding:
        case ST_Y:
        case ST_YOft:
        case ST_Height:
            return mValue.abs(wnd.getPixelSize().mHeight);
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("unknown or unsupported SizeType encountered."));
            break;
        }
    }
    //------------------------------------------------------------------------
    NIIf RelationSize::getValue(const Widget &, const Rectf & container) const
    {
        switch (mType)
        {
        case ST_LeftPadding:
        case ST_RightPadding:
        case ST_X:
        case ST_XOft:
        case ST_Width:
            return mValue.abs(container.getWidth());
        case ST_TopPadding:
        case ST_BottomPadding:
        case ST_Y:
        case ST_YOft:
        case ST_Height:
            return mValue.abs(container.getHeight());
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("²»ÏêÀàÐÍ! "));
            break;
        }
    }
    //------------------------------------------------------------------------
    SizeBase * RelationSize::clone() const
    {
        RelationSize * ndim = N_new RelationSize(mValue, mType);
        return ndim;
    }
    //------------------------------------------------------------------------
    void RelationSize::writeTag(XmlSerializer * out) const
    {
        out->begin(_T("Rel"));
    }
    //------------------------------------------------------------------------
    void RelationSize::writeValue(XmlSerializer * out) const
    {
        if (mValue.mScale != 0)
            out->attribute(_T("scale"), N_conv(mValue.mScale));

        if (mValue.mOffset != 0)
            out->attribute(_T("offset"), N_conv(mValue.mOffset));
        out->attribute(_T("value"), N_conv(mType));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // AbsoluteSize
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    AbsoluteSize::AbsoluteSize()
    {
    }
    //------------------------------------------------------------------------
    AbsoluteSize::AbsoluteSize(NIIf val) :
        mValue(val)
    {
    }
    //------------------------------------------------------------------------
    NIIf AbsoluteSize::getValue(const Widget &) const
    {
        return mValue;
    }
    //------------------------------------------------------------------------
    NIIf AbsoluteSize::getValue(const Widget &, const Rectf &) const
    {
        return mValue;
    }
    //------------------------------------------------------------------------
    SizeBase * AbsoluteSize::clone() const
    {
        return N_new AbsoluteSize(*this);
    }
    //------------------------------------------------------------------------
    void AbsoluteSize::writeTag(XmlSerializer * out) const
    {
        out->begin(_T("Abs"));
    }
    //------------------------------------------------------------------------
    void AbsoluteSize::writeValue(XmlSerializer * out) const
    {
        out->attribute(_T("value"), N_conv(mValue));
    }
    //------------------------------------------------------------------------
}