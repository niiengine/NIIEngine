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
#include "NiiUIStyleXmlAnalyze.h"
#include "NiiUIStyleWidget.h"
#include "NiiUIStyleSection.h"
#include "NiiAnimationCollectorXmlAnalyze.h"
#include "NiiUIStyleSpecial.h"
#include "NiiUIStrConv.h"
#include "NiiFontManager.h"
#include "NiiPixelManager.h"
#include "NiiColour.h"
#include "NiiUIStrConv.h"
#include "NiiXmlAnalyzeList.h"

#include "NiiUIButton.h"
#include "NiiUITabControl.h"
#include "NiiUINumSpinner.h"
#include "NiiUIListContainer.h"
#include "NiiUIListHeader.h"
#include "NiiUIMultiColumnList.h"
#include "NiiListenerEvent.h"
#include "NiiUIWidgetManager.h"

#include <sstream>

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // OperatorSize
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    OperatorSize::OperatorSize() :
        mLeft(0),
        mRight(0),
        mOp(SOT_Unknow)
    {
    }
    //------------------------------------------------------------------------
    OperatorSize::OperatorSize(SizeOpType op) :
        mLeft(0),
        mRight(0),
        mOp(op)
    {
    }
    //------------------------------------------------------------------------
    OperatorSize::OperatorSize(SizeOpType op, SizeBase * left, SizeBase * right) :
        mLeft(left ? left->clone() : 0),
        mRight(right ? right->clone() : 0),
        mOp(op)
    {
    }
    //------------------------------------------------------------------------
    OperatorSize::~OperatorSize()
    {
        N_delete mRight;
        N_delete mLeft;
    }
    //------------------------------------------------------------------------
    void OperatorSize::setFirst(const SizeBase * value)
    {
        if(mLeft)
            N_delete mLeft;

        mLeft = value ? value->clone() : 0;
    }
    //------------------------------------------------------------------------
    void OperatorSize::setSecond(const SizeBase * value)
    {
        if(mRight)
            N_delete mRight;

        mRight = value ? value->clone() : 0;
    }
    //------------------------------------------------------------------------
    void OperatorSize::setNextOperand(SizeBase * operand, bool copy)
    {
        if(!mLeft)
            mLeft = operand ? (copy ? operand->clone() : operand) : 0;
        else if (!mRight)
            mRight = operand ? (copy ? operand->clone() : operand) : 0;
        else
            N_EXCEPT(InvalidRequest, _I18n("Both operands are already set."));
    }
    //------------------------------------------------------------------------
    NIIf OperatorSize::getValue(const Widget & wnd) const
    {
        NIIf lval = mLeft ? mLeft->getValue(wnd) : 0.0f;
        NIIf rval = mRight ? mRight->getValue(wnd) : 0.0f;

        return getValueImpl(lval, rval);
    }
    //------------------------------------------------------------------------
    NIIf OperatorSize::getValue(const Widget & wnd, const Rectf & container) const
    {
        NIIf lval = mLeft ? mLeft->getValue(wnd, container) : 0.0f;
        NIIf rval = mRight ? mRight->getValue(wnd, container) : 0.0f;

        return getValueImpl(lval, rval);
    }
    //------------------------------------------------------------------------
    NIIf OperatorSize::getValueImpl(NIIf lval, NIIf rval) const
    {
        switch(mOp)
        {
        case SOT_Unknow:
            return 0.0f;
        case SOT_Add:
            return lval + rval;
        case SOT_Subtract:
            return lval - rval;
        case SOT_Multiply:
            return lval * rval;
        // divide by zero returns zero.  Not 100% correct but is better than the
        // alternatives in the majority of cases where Styles are concerned.
        case SOT_Divide:
            return rval == 0.0f ? rval : lval / rval;
        default:
            N_EXCEPT(InvalidRequest, _I18n("Unknown SizeOpType value."));
        }
    }
    //------------------------------------------------------------------------
    SizeBase * OperatorSize::clone() const
    {
        return N_new OperatorSize(mOp, mLeft, mRight);
    }
    //------------------------------------------------------------------------
    void OperatorSize::write(XmlSerializer * out) const
    {
        writeTag(out);
        writeValue(out);

        if(mLeft)
            mLeft->write(out);

        if(mRight)
            mRight->write(out);

        out->end();
    }
    //------------------------------------------------------------------------
    void OperatorSize::writeTag(XmlSerializer * out) const
    {
        out->begin(_T("Operator"));
    }
    //------------------------------------------------------------------------
    void OperatorSize::writeValue(XmlSerializer * out) const
    {
        out->attribute(_T("type"), NUI_conv(mOp));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ImageSizeBase
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ImageSizeBase::ImageSizeBase(SizeType type) :
        SizeBase(type)
    {
    }
    //------------------------------------------------------------------------
    NIIf ImageSizeBase::getValue(const Widget & wnd) const
    {
        const PixelBuffer * pb = getSourceImage(wnd);

        if (!pb)
            return 0.0f;

        switch (mType)
        {
        case ST_Width:
            return pb->getSize().mWidth;
            break;
        case ST_Height:
            return pb->getSize().mHeight;
            break;
        case ST_XOft:
            return pb->getOffset().x;
            break;
        case ST_YOft:
            return pb->getOffset().y;
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("unknown or unsupported SizeType encountered."));
            break;
        }
    }
    //------------------------------------------------------------------------
    NIIf ImageSizeBase::getValue(const Widget & wnd, const Rectf &) const
    {
        // This size type does not alter when whithin a container Rect.
        return getValue(wnd);
    }
    //------------------------------------------------------------------------
    void ImageSizeBase::writeValue(XmlSerializer * out) const
    {
        out->attribute(_T("value"), NUI_conv(mType));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ImageSize
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ImageSize::ImageSize(const String & src, SizeType type) :
        ImageSizeBase(type),
        mImageSrc(src)
    {
    }
    //------------------------------------------------------------------------
    const PixelBuffer * ImageSize::getSourceImage(const Widget &) const
    {
        StringList temp;
        StrUtil::split(mImageSrc, temp, _T("|"));
        Nui32 rid, pid;
        StrConv::conv(temp[0], rid);
        StrConv::conv(temp[1], pid);
        return N_Only(Pixel).get(rid, pid);
    }
    //------------------------------------------------------------------------
    SizeBase * ImageSize::clone() const
    {
        return N_new ImageSize(*this);
    }
    //------------------------------------------------------------------------
    void ImageSize::writeTag(XmlSerializer * dest) const
    {
        dest->begin(_T("PixelSize"));
    }
    //------------------------------------------------------------------------
    void ImageSize::writeValue(XmlSerializer * dest) const
    {
        ImageSizeBase::writeValue(dest);
        dest->attribute(_T("name"), mImageSrc);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ImagePropertySize
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ImagePropertySize::ImagePropertySize(PropertyID pid, SizeType dim) :
        ImageSizeBase(dim),
        mPropertyID(pid)
    {
    }
    //------------------------------------------------------------------------
    const PixelBuffer * ImagePropertySize::getSourceImage(const Widget & widget) const
    {
        String image_name;
        widget.get(mPropertyID, image_name);
        StringList temp;
        StrUtil::split(image_name, temp, _T("|"));
        Nui32 rid, pid;
        StrConv::conv(temp[1], rid);
        StrConv::conv(temp[2], pid);
        return image_name.empty() ? 0 : N_Only(Pixel).get(rid, pid);
    }
    //------------------------------------------------------------------------
    SizeBase * ImagePropertySize::clone() const
    {
        return N_new ImagePropertySize(*this);
    }
    //------------------------------------------------------------------------
    void ImagePropertySize::writeTag(XmlSerializer * dest) const
    {
        dest->begin(_T("ImagePropertySize"));
    }
    //------------------------------------------------------------------------
    void ImagePropertySize::writeValue(XmlSerializer * dest) const
    {
        ImageSizeBase::writeValue(dest);
        dest->attribute(_T("name"), mPropertyID);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // WidgetSize
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    WidgetSize::WidgetSize(WidgetID wid, SizeType type) :
        SizeBase(type),
        mWidgetID(wid)
    {
    }
    //------------------------------------------------------------------------
    NIIf WidgetSize::getValue(const Widget & wnd) const
    {
        const Widget * widget;

        // if target widget name is empty, then use the input window.
        if(mWidgetID == 0)
        {
            widget = &wnd;
        }
        // name not empty, so find window with required name
        else
        {
            widget = static_cast<const Container &>(wnd).get(mWidgetID);
        }

        // get size of parent; required to extract pixel values
        PlaneSizef parentSize(widget->calcParentArea());

        switch(mType)
        {
        case ST_Width:
            return widget->getPixelSize().mWidth;
            break;
        case ST_Height:
            return widget->getPixelSize().mHeight;
            break;
        case ST_XOft:
            N_Only(Log).log(_I18n("Nonsensical SizeType of ST_XOft specified!  returning 0.0f"));
            return 0.0f;
            break;
        case ST_YOft:
            N_Only(Log).log(_I18n("Nonsensical SizeType of ST_YOft specified!  returning 0.0f"));
            return 0.0f;
            break;
        case ST_LeftPadding:
        case ST_X:
            return widget->getPos().x.abs(parentSize.mWidth);
            break;
        case ST_TopPadding:
        case ST_Y:
            return widget->getPos().y.abs(parentSize.mHeight);
            break;
        case ST_RightPadding:
            return widget->getRelArea().mRight.abs(parentSize.mWidth);
            break;
        case ST_BottomPadding:
            return widget->getRelArea().mBottom.abs(parentSize.mHeight);
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("unknown or unsupported SizeType encountered."));
            break;
        }
    }
    //------------------------------------------------------------------------
    NIIf WidgetSize::getValue(const Widget & wnd, const Rectf &) const
    {
        // This size type does not alter when whithin a container Rect.
        return getValue(wnd);
    }
    //------------------------------------------------------------------------
    SizeBase * WidgetSize::clone() const
    {
        return N_new WidgetSize(*this);
    }
    //------------------------------------------------------------------------
    void WidgetSize::writeTag(XmlSerializer * dest) const
    {
        dest->begin(_T("WidgetSize"));
    }
    //------------------------------------------------------------------------
    void WidgetSize::writeValue(XmlSerializer * dest) const
    {
        if(mWidgetID != 0)
            dest->attribute(_T("widget"), mWidgetID);

        dest->attribute(_T("value"), NUI_conv(mType));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // FontSize
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    FontSize::FontSize(WidgetID id, FontID font, const String & text,
        FontSizeType metric, NIIf padding) :
            mFontID(font),
            mText(text),
            mWidgetID(id),
            mMetricType(metric),
            mPadding(padding)
    {
    }
    //------------------------------------------------------------------------
    NIIf FontSize::getValue(const Widget & wnd) const
    {
        // get window to use.
        const Widget * widget = mWidgetID == 0 ? &wnd :
            static_cast<const Container &>(wnd).get(mWidgetID);
        // get font to use
        const Font * font = (mFontID == 0 ? widget->getFont() : N_Only(Font).getFont(mFontID));

        if(font)
        {
            switch (mMetricType)
            {
            case FST_LineSpace:
                return font->getLineSpacing() + mPadding;
                break;
            case FST_BaseLine:
                return font->getBaseline() + mPadding;
                break;
            case FST_TextEntent:
                return font->getTextExtent(mText.empty() ? widget->getText() : mText) + mPadding;
                break;
            default:
                N_EXCEPT(InvalidRequest, _I18n("unknown or unsupported FontSizeType encountered."));
                break;
            }
        }
        // no font, return padding value only.
        else
        {
            return mPadding;
        }
    }
    //------------------------------------------------------------------------
    NIIf FontSize::getValue(const Widget & wnd, const Rectf &) const
    {
        return getValue(wnd);
    }
    //------------------------------------------------------------------------
    SizeBase * FontSize::clone() const
    {
        return N_new FontSize(*this);
    }
    //------------------------------------------------------------------------
    void FontSize::writeTag(XmlSerializer * dest) const
    {
        dest->begin(_T("FontSize"));
    }
    //------------------------------------------------------------------------
    void FontSize::writeValue(XmlSerializer * dest) const
    {
        if(mWidgetID != 0)
            dest->attribute(_T("widget"), N_conv(mWidgetID));

        if(mFontID != 0)
            dest->attribute(_T("font"), N_conv(mFontID));

        if(!mText.empty())
            dest->attribute(_T("string"), mText);

        if(mPadding != 0)
            dest->attribute(_T("padding"), N_conv(mPadding));

        dest->attribute(_T("type"), NUI_conv(mMetricType));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PropertySize
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PropertySize::PropertySize(WidgetID wid, PropertyID pid, SizeType type) :
        SizeBase(type),
        mPropertyID(pid),
        mWidgetID(wid)
    {
    }
    //------------------------------------------------------------------------
    NIIf PropertySize::getValue(const Widget & e) const
    {
        // get window to use.
        const Widget * src = mWidgetID == 0 ? &e : static_cast<const Container &>(e).get(mWidgetID);

        if(mType == ST_Unknow)
        {
            NIIf temp;
            src->get(mPropertyID, &temp);
            return temp;
        }

        RelPosf d;
        src->get(mPropertyID, &d);
        PlaneSizef s = src->getPixelSize();

        switch(mType)
        {
        case ST_Width:
            return d.abs(s.mWidth);
        case ST_Height:
            return d.abs(s.mHeight);
        default:
            N_EXCEPT(InvalidRequest, _I18n("unknown or unsupported SizeType encountered."));
        }
    }
    //------------------------------------------------------------------------
    NIIf PropertySize::getValue(const Widget & wnd, const Rectf &) const
    {
        return getValue(wnd);
    }
    //------------------------------------------------------------------------
    SizeBase * PropertySize::clone() const
    {
        return N_new PropertySize(*this);
    }
    //------------------------------------------------------------------------
    void PropertySize::writeTag(XmlSerializer * dest) const
    {
        dest->begin(_T("PropertySize"));
    }
    //------------------------------------------------------------------------
    void PropertySize::writeValue(XmlSerializer * dest) const
    {
        if(mWidgetID != 0)
            dest->attribute(_T("widget"), N_conv(mWidgetID));
        dest->attribute(_T("name"), N_conv(mPropertyID));
        if(mType != ST_Unknow)
            dest->attribute(_T("type"), NUI_conv(mType));
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleXmlAnalyze
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleXmlAnalyze::StyleXmlAnalyze(LangID lid) :
        XmlAnalyze(0, N_Script_UIStyle, _T("Style.xsd"), lid),
        mStyle(0),
        mStyleWidget(0),
        mStyleSection(0),
        mStyleLayout(0),
        mLayer(0),
        mSection(0),
        mImageryCom(0),
        mComArea(0),
        mTextCom(0),
        mStyleSpecial(0),
        mFrameCom(0),
        mListenerProperty(0),
        mListenerEvent(0)
    {
        mStyleManager = N_OnlyPtr(Widget);
        // register element start handlers
        setBeginPrc(StyleUnitProperty,          (PrcCB)&StyleXmlAnalyze::PrcStyleCB);
        setBeginPrc(WidgetStyleUnitProperty,    (PrcCB)&StyleXmlAnalyze::PrcWidgetStyleCB);
        setBeginPrc(StyleWidgetUnitProperty,    (PrcCB)&StyleXmlAnalyze::PrcStyleWidgetCB);
        setBeginPrc(StyleSectionUnitProperty,   (PrcCB)&StyleXmlAnalyze::PrcStyleSectionCB);
        setBeginPrc(StyleLayoutUnitProperty,    (PrcCB)&StyleXmlAnalyze::PrcStyleLayoutCB);
        setBeginPrc(LayerUnitProperty,          (PrcCB)&StyleXmlAnalyze::PrcLayerCB);
        setBeginPrc(SectionUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcSectionCB);
        setBeginPrc(SectionImageUnitProperty,   (PrcCB)&StyleXmlAnalyze::PrcImageryComCB);
        setBeginPrc(SectionTextUnitProperty,    (PrcCB)&StyleXmlAnalyze::PrcTextComCB);
        setBeginPrc(SectionFrameUnitProperty,   (PrcCB)&StyleXmlAnalyze::PrcFrameComCB);
        setBeginPrc(ComAreaUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcComAreaCB);
        setBeginPrc(ComPixelUnitProperty,       (PrcCB)&StyleXmlAnalyze::PrcComPixelCB);
        setBeginPrc(ColoursUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcColoursCB);
        setBeginPrc(VLayoutUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcVLayoutCB);
        setBeginPrc(HLayoutUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcHLayoutCB);
        setBeginPrc(VAlignUnitProperty,         (PrcCB)&StyleXmlAnalyze::PrcVAlignCB);
        setBeginPrc(HAlignUnitProperty,         (PrcCB)&StyleXmlAnalyze::PrcHAlignCB);
        setBeginPrc(PropertyUnitProperty,       (PrcCB)&StyleXmlAnalyze::PrcPropertyCB);
        setBeginPrc(SizeDefineProperty,         (PrcCB)&StyleXmlAnalyze::PrcSizeDefineCB);
        setBeginPrc(RelSizeUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcRelSizeCB);
        setBeginPrc(AbsSizeUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcAbsSizeCB);
        setBeginPrc(PixelSizeUnitProperty,      (PrcCB)&StyleXmlAnalyze::PrcPixelSizeCB);
        setBeginPrc(PixelSizeIDUnitProperty,    (PrcCB)&StyleXmlAnalyze::PrcPixelSizeIDCB);
        setBeginPrc(WidgetSizeUnitProperty,     (PrcCB)&StyleXmlAnalyze::PrcWidgetSizeCB);
        setBeginPrc(FontSizeUnitProperty,       (PrcCB)&StyleXmlAnalyze::PrcFontSizeCB);
        setBeginPrc(SizeIDUnitProperty,         (PrcCB)&StyleXmlAnalyze::PrcSizeIDCB);
        setBeginPrc(TextUnitProperty,           (PrcCB)&StyleXmlAnalyze::PrcTextCB);
        setBeginPrc(ColoursIDUnitProperty,      (PrcCB)&StyleXmlAnalyze::PrcColoursIDCB);
        setBeginPrc(StyleSpecialUnitProperty,   (PrcCB)&StyleXmlAnalyze::PrcStyleSpecialCB);
        setBeginPrc(StylePropertyUnitProperty,  (PrcCB)&StyleXmlAnalyze::PrcStylePropertyCB);
        setBeginPrc(ListenerPropertyUnitProperty,   (PrcCB)&StyleXmlAnalyze::PrcListenerPropertyCB);
        setBeginPrc(SizeOpUnitProperty,             (PrcCB)&StyleXmlAnalyze::PrcSizeOpCB);
        setBeginPrc(VLayoutIDUnitProperty,          (PrcCB)&StyleXmlAnalyze::PrcVLayoutIDCB);
        setBeginPrc(HLayoutIDUnitProperty,          (PrcCB)&StyleXmlAnalyze::PrcHLayoutIDCB);
        setBeginPrc(ComAreaIDUnitProperty,          (PrcCB)&StyleXmlAnalyze::PrcComAreaIDCB);
        setBeginPrc(PixelIDUnitProperty,            (PrcCB)&StyleXmlAnalyze::PrcPixelIDCB);
        setBeginPrc(TextIDUnitProperty,             (PrcCB)&StyleXmlAnalyze::PrcTextIDCB);
        setBeginPrc(FontIDUnitProperty,             (PrcCB)&StyleXmlAnalyze::PrcFontIDCB);
        setBeginPrc(ColourUnitProperty,             (PrcCB)&StyleXmlAnalyze::PrcColourCB);
        setBeginPrc(ListenerTargetUnitProperty,     (PrcCB)&StyleXmlAnalyze::PrcListenerTargetCB);
        setBeginPrc(N_XML_TAG_AnimationUnit,        (PrcCB)&StyleXmlAnalyze::PrcAnimationCB);
        setBeginPrc(ListenerEventUnitProperty,      (PrcCB)&StyleXmlAnalyze::PrcListenerEventCB);
        setBeginPrc(EventTargetUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcEventTargetCB);
        setBeginPrc(StyleSpecialIDUnitProperty,     (PrcCB)&StyleXmlAnalyze::PrcStyleSpecialIDCB);

        setEndPrc(StyleUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcStyleUnitECB);
        setEndPrc(WidgetStyleUnitProperty,  (PrcCB)&StyleXmlAnalyze::PrcWidgetStyleECB);
        setEndPrc(StyleWidgetUnitProperty,  (PrcCB)&StyleXmlAnalyze::PrcStyleWidgetECB);
        setEndPrc(StyleSectionUnitProperty, (PrcCB)&StyleXmlAnalyze::PrcStyleSectionECB);
        setEndPrc(StyleLayoutUnitProperty,  (PrcCB)&StyleXmlAnalyze::PrcStyleLayoutECB);
        setEndPrc(LayerUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcLayerECB);
        setEndPrc(SectionUnitProperty,      (PrcCB)&StyleXmlAnalyze::PrcSectionECB);
        setEndPrc(SectionImageUnitProperty, (PrcCB)&StyleXmlAnalyze::PrcImageryComECB);
        setEndPrc(SectionTextUnitProperty,  (PrcCB)&StyleXmlAnalyze::PrcTextComECB);
        setEndPrc(SectionFrameUnitProperty, (PrcCB)&StyleXmlAnalyze::PrcFrameComECB);
        setEndPrc(ComAreaUnitProperty,      (PrcCB)&StyleXmlAnalyze::PrcComAreaECB);
        setEndPrc(RelSizeUnitProperty,      (PrcCB)&StyleXmlAnalyze::PrcAnySizeECB);
        setEndPrc(AbsSizeUnitProperty,      (PrcCB)&StyleXmlAnalyze::PrcAnySizeECB);
        setEndPrc(PixelSizeUnitProperty,    (PrcCB)&StyleXmlAnalyze::PrcAnySizeECB);
        setEndPrc(PixelSizeIDUnitProperty,  (PrcCB)&StyleXmlAnalyze::PrcAnySizeECB);
        setEndPrc(WidgetSizeUnitProperty,   (PrcCB)&StyleXmlAnalyze::PrcAnySizeECB);
        setEndPrc(FontSizeUnitProperty,     (PrcCB)&StyleXmlAnalyze::PrcAnySizeECB);
        setEndPrc(SizeIDUnitProperty,       (PrcCB)&StyleXmlAnalyze::PrcAnySizeECB);
        setEndPrc(SizeOpUnitProperty,       (PrcCB)&StyleXmlAnalyze::PrcAnySizeECB);
        setEndPrc(StyleSpecialUnitProperty, (PrcCB)&StyleXmlAnalyze::PrcStyleSpecialECB);
        setEndPrc(ListenerPropertyUnitProperty,     (PrcCB)&StyleXmlAnalyze::PrcListenerPropertyECB);
        setEndPrc(ListenerEventUnitProperty,        (PrcCB)&StyleXmlAnalyze::PrcListenerEventECB);
    }
    //------------------------------------------------------------------------
    StyleXmlAnalyze::~StyleXmlAnalyze()
    {
    }
    //------------------------------------------------------------------------
    const String & StyleXmlAnalyze::getXSD() const
    {
        return mXSD;
    }
    //------------------------------------------------------------------------
    XmlUnitID StyleXmlAnalyze::getUnit() const
    {
        return N_XML_TAG_UIStyle;
    }
    //------------------------------------------------------------------------
    GroupID StyleXmlAnalyze::getGroup() const
    {
        return N_Only(Widget).getStyleGroup();
    }
    //------------------------------------------------------------------------
    Nui32 StyleXmlAnalyze::hexStringToARGB(const String & str)
    {
        Nui32 val;
        Nistringstream s(str.c_str());
        s >> std::hex >> val;

        return val;
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcTagUnitCB(const PropertyData * pdc)
    {

    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::assignColours(const ColourArea & cols)
    {
        if(mFrameCom)
        {
            mFrameCom->setColour(cols);
        }
        else if(mImageryCom)
        {
            mImageryCom->setColour(cols);
        }
        else if(mTextCom)
        {
            mTextCom->setColour(cols);
        }
        else if(mStyleSection)
        {
            mStyleSection->setColour(cols);
        }
        else if(mSection)
        {
            mSection->setColour(cols);
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleCB(const PropertyData * pdc)
    {
        N_Only(Log).log(_I18n("===== Style 'root' element: widget_style and feel parsing begins ====="));

        Nui32 version;
        pdc->get(VersionProperty, version);

        if(version != mVersion)
        {
            N_EXCEPT(InvalidRequest, _I18n("风格序列版本错误! "));
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcWidgetStyleCB(const PropertyData * pdc)
    {
        N_assert(mStyle == 0, "error logic");
        Nui32 id, pid;
        pdc->get(IDProperty, id);
        pdc->get(InheritsAttribute, pid);
        mStyle = N_new Style(id, pid);

        N_Only(Log).log(_I18n("---> Start of definition for widget widget_style '") +
            N_conv(mStyle->getID()) + _T("'."));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleWidgetCB(const PropertyData * pdc)
    {
        N_assert(mStyleWidget == 0, "error logic");
        Nui32 id;
        Nui32 tid;
        bool tbool;
        pdc->get(TypeProperty, tid);
        pdc->get(IDProperty, id);
        pdc->get(AutoWindowAttribute, tbool, true);
        mStyleWidget = N_new StyleWidget(tid, id, tbool);

        N_Only(Log).log(_I18n("-----> Start of definition for child widget.")
            _I18n(" Type: ") + N_conv(mStyleWidget->getViewID()) +
            _I18n(" Name: ") + N_conv(mStyleWidget->getID()) +
            _I18n(" Auto: ") + (mStyleWidget->isAutoWindow() ? _I18n("Yes") : _I18n("No")));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleSectionCB(const PropertyData * pdc)
    {
        N_assert(mStyleSection == 0, "error logic");
        Nui32 id;
        pdc->get(IDProperty, id);
        mStyleSection = N_new StyleSection(id);

        N_Only(Log).log(_I18n("-----> Start of definition for imagery section '") +
            N_conv(mStyleSection->getID()) + _T("'."));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleLayoutCB(const PropertyData * pdc)
    {
        N_assert(mStyleLayout == 0, "error logic");
        bool tbool;
        Nui32 t32;
        pdc->get(ClippedAttribute, tbool, true);
        pdc->get(NameProperty, t32);
        mStyleLayout = N_new StyleLayout(t32);
        mStyleLayout->setClip(tbool);

        N_Only(Log).log(_I18n("-----> Start of definition for imagery for state '") +
            N_conv(mStyleLayout->getID()) + _T("'."));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcLayerCB(const PropertyData * pdc)
    {
        N_assert(mLayer == 0, "error logic");
        Nui32 ti;
        pdc->get(PriorityAttribute, ti, 0);
        mLayer = N_new Layer(ti);

        N_Only(Log).log(_I18n("-------> Start of definition of new imagery layer, priority: ") + N_conv(ti));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcSectionCB(const PropertyData * pdc)
    {
        N_assert(mSection == 0, "error logic");
        N_assert(mStyle != 0, "error logic");
        Nui32 owner, sid, cp, wid;
        pdc->get(WidgetStyleAttribute, owner);
        pdc->get(IDProperty, sid);
        pdc->get(ControlPropertyAttribute, cp);
        pdc->get(ControlWidgetAttribute, wid);
        mSection = N_new Section(owner== 0 ? mStyle->getID() : owner,
            sid, cp, wid);

        N_Only(Log).log(_I18n("---------> Layer references imagery section '") +
            N_conv(mSection->getStyleSection()) + _T("'."));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcImageryComCB(const PropertyData *)
    {
        N_assert(mImageryCom == 0, "error logic");
        mImageryCom = N_new StyleSectionImageUnit();

        N_Only(Log).log(_I18n("-------> Pixel component definition..."));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcTextComCB(const PropertyData *)
    {
        N_assert(mTextCom == 0, "error logic");
        mTextCom = N_new StyleSectionTextUnit();

        N_Only(Log).log(_I18n("-------> Text component definition..."));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcFrameComCB(const PropertyData *)
    {
        N_assert(mFrameCom == 0, "error logic");
        mFrameCom = N_new StyleSectionFrameUnit();

        N_Only(Log).log(_I18n("-------> Frame component definition..."));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcComAreaCB(const PropertyData * )
    {
        N_assert(mComArea == 0, "error logic");
        mComArea = N_new StyleArea();
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcComPixelCB(const PropertyData * pdc)
    {
        PixelBuffer * pbuffer;
        if(mImageryCom)
        {
            Nui32 pid;
            pdc->get(PixelIDProperty, pid);
            pbuffer = N_Only(Pixel).get(pid);
            mImageryCom->setImage(pbuffer);
            N_Only(Log).log(_I18n("模块: ") + pid);
        }
        else if(mFrameCom)
        {
            Nui32 pid;
            FrameComType temp;
            pdc->get(PixelIDProperty, pid);
            UI::StrConv::conv(pdc->get(TypeProperty), temp);
            pbuffer = N_Only(Pixel).get(pid);
            mFrameCom->setImage(temp, pbuffer);

            N_Only(Log).log(_I18n("模块: ") + N_conv(pid) + _T(" for: ") +
                pdc->get(TypeProperty));
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcColoursCB(const PropertyData * pdc)
    {
        ColourArea cols(
            hexStringToARGB(pdc->get(TopLeftAttribute)),
            hexStringToARGB(pdc->get(TopRightAttribute)),
            hexStringToARGB(pdc->get(BottomLeftAttribute)),
            hexStringToARGB(pdc->get(BottomRightAttribute)));

        assignColours(cols);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcVLayoutCB(const PropertyData * pdc)
    {
        if(mFrameCom)
        {
            VLayout fmt;
            FrameComType what;
            String temp;
            pdc->get(TypeProperty, temp, _T("Bg"));
            UI::StrConv::conv(temp, what);
            UI::StrConv::conv(pdc->get(TypeProperty), fmt);

            switch(what)
            {
            case FCT_Left:
                mFrameCom->setLeftLayout(fmt);
                break;
            case FCT_Right:
                mFrameCom->setRightLayout(fmt);
                break;
            case FCT_Bg:
                mFrameCom->setBgVLayout(fmt);
                break;
            default:
                N_EXCEPT(InvalidRequest, VLayoutUnitProperty + _I18n(" within ") +
                    N_conv(SectionFrameUnitProperty) + _I18n(" may only be used for ")
                    _I18n("LeftEdge, RightEdge or Background components. Received: ") +
                    pdc->get(TypeProperty));
            }
        }
        else if(mImageryCom)
        {
            VLayout layout;
            UI::StrConv::conv(pdc->get(TypeProperty), layout);
            mImageryCom->setVLayout(layout);
        }
        else if(mTextCom)
        {
            VTextLayout layout;
            UI::StrConv::conv(pdc->get(TypeProperty), layout);
            mTextCom->setVLayout(layout);
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcHLayoutCB(const PropertyData * pdc)
    {
        if(mFrameCom)
        {
            FrameComType what;
            HLayout fmt;
            UI::StrConv::conv(pdc->getDefault(TypeProperty, _T("Bg")), what);
            UI::StrConv::conv(pdc->get(TypeProperty), fmt);

            switch(what)
            {
            case FCT_Top:
                mFrameCom->setTopLayout(fmt);
                break;
            case FCT_Bottom:
                mFrameCom->setBottomLayout(fmt);
                break;
            case FCT_Bg:
                mFrameCom->setBgHLayout(fmt);
                break;
            default:
                N_EXCEPT(InvalidRequest, HLayoutUnitProperty + _I18n(" within ") +
                    N_conv(SectionFrameUnitProperty) + _I18n(" may only be used for ")
                    _I18n("TopEdge, BottomEdge or Background components. Received: ") +
                    pdc->get(TypeProperty));
            }
        }
        else if(mImageryCom)
        {
            HLayout layout;
            UI::StrConv::conv(pdc->get(TypeProperty), layout);
            mImageryCom->setHLayout(layout);
        }
        else if(mTextCom)
        {
            HTextLayout layout;
            UI::StrConv::conv(pdc->get(TypeProperty), layout);
            mTextCom->setHLayout(layout);
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcVAlignCB(const PropertyData * pdc)
    {
        N_assert(mStyleWidget != 0, "error logic");
        VAlign temp;
        UI::StrConv::conv(pdc->get(TypeProperty), temp);
        mStyleWidget->setVAlign(temp);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcHAlignCB(const PropertyData * pdc)
    {
        N_assert(mStyleWidget != 0, "error logic");
        HAlign temp;
        UI::StrConv::conv(pdc->get(TypeProperty), temp);
        mStyleWidget->setHAlign(temp);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcPropertyCB(const PropertyData * pdc)
    {
        N_assert(mStyle != 0, "error logic");
        Nui32 id;
        pdc->get(IDProperty, id);
        AssignProperty prop(id, pdc->get(ValueProperty));

        if(mStyleWidget)
        {
            mStyleWidget->add(prop);
            N_Only(Log).log(_I18n("-------> Added property initialiser for property: ") +
                N_conv(prop.getProperty()) + _I18n(" with value: ") + prop.getValue());
        }
        else
        {
            mStyle->add(prop);
            N_Only(Log).log(_I18n("---> Added property initialiser for property: ") +
                N_conv(prop.getProperty()) + _I18n(" with value: ") + prop.getValue());
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcSizeDefineCB(const PropertyData * pdc)
    {
        SizeType temp;
        UI::StrConv::conv(pdc->get(TypeProperty), temp);
        mSizeDefine.setType(temp);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcRelSizeCB(const PropertyData * pdc)
    {
        NIIf tf1, tf2;
        pdc->get(ScaleAttribute, tf1, 0.0f);
        pdc->get(OffsetAttribute, tf2, 0.0f);
        SizeType temp;
        UI::StrConv::conv(pdc->get(ValueProperty), temp);
        RelationSize * base = N_new RelationSize(RelPosf(tf1, tf2), temp);
        mSizeList.push_back(base);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcAbsSizeCB(const PropertyData * pdc)
    {
        NIIf tf;
        pdc->get(ValueProperty, tf, 0.0f);
        AbsoluteSize * base = N_new AbsoluteSize(tf);

        mSizeList.push_back(base);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcPixelSizeCB(const PropertyData * pdc)
    {
        SizeType temp;
        UI::StrConv::conv(pdc->get(ValueProperty), temp);
        ImageSize * base = N_new ImageSize(pdc->get(NameProperty), temp);

        mSizeList.push_back(base);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcPixelSizeIDCB(const PropertyData * pdc)
    {
        Nui32 pid;
        pdc->get(NameProperty, pid);
        SizeType temp;
        UI::StrConv::conv(pdc->get(ValueProperty), temp);
        ImagePropertySize * base = N_new ImagePropertySize(pid, temp);
        mSizeList.push_back(base);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcWidgetSizeCB(const PropertyData * pdc)
    {
        Nui32 wid;
        SizeType temp;
        pdc->get(WidgetAttribute, wid);
        UI::StrConv::conv(pdc->get(ValueProperty), temp);

        WidgetSize * base = N_new WidgetSize(wid, temp);

        mSizeList.push_back(base);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcFontSizeCB(const PropertyData * pdc)
    {
        NIIf tf;
        Nui32 wid, fid;
        pdc->get(WidgetAttribute, wid);
        pdc->get(FontAttribute, fid);
        pdc->get(PaddingAttribute, tf);
        FontSizeType temp;
        UI::StrConv::conv(pdc->get(TypeProperty), temp);
        FontSize * base = N_new FontSize(wid, fid, pdc->get(StringAttribute), temp, tf);

        mSizeList.push_back(base);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcSizeIDCB(const PropertyData * pdc)
    {
        Nui32 wid, pid;
        SizeType type;
        String str_type = pdc->get(TypeProperty);
        if(!str_type.empty())
            UI::StrConv::conv(str_type, type, ST_Unknow);
        pdc->get(WidgetAttribute, wid);
        pdc->get(NameProperty, pid);
        PropertySize * base = N_new PropertySize(wid, pid, type);

        mSizeList.push_back(base);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcTextCB(const PropertyData * pdc)
    {
        N_assert (mTextCom != 0, "error logic");
        mTextCom->setText(pdc->get(StringAttribute));
        Nui32 tempid;
        pdc->get(FontAttribute, tempid);
        mTextCom->setFont(tempid);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcColoursIDCB(const PropertyData * pdc)
    {
        // need to decide what to apply colours to
        if(mFrameCom)
        {
            Nui32 tempid;
            pdc->get(IDProperty, tempid);
            mFrameCom->setColour(tempid);
        }
        else if(mImageryCom)
        {
            Nui32 tempid;
            pdc->get(IDProperty, tempid);
            mImageryCom->setColour(tempid);
        }
        else if(mTextCom)
        {
            Nui32 tempid;
            pdc->get(IDProperty, tempid);
            mTextCom->setColour(tempid);
        }
        else if(mStyleSection)
        {
            Nui32 tempid;
            pdc->get(IDProperty, tempid);
            mStyleSection->setColourFetch(tempid);
        }
        else if(mSection)
        {
            Nui32 tempid;
            pdc->get(IDProperty, tempid);
            mSection->setColourID(tempid);
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleSpecialCB(const PropertyData * pdc)
    {
        N_assert(mStyleSpecial == 0, "error logic");
        Nui32 pid;
        pdc->get(IDProperty, pid);
        mStyleSpecial = N_new StyleSpecial(pid);

        N_Only(Log).log(_I18n("-----> Creating named area: ") + mStyleSpecial->getID());
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStylePropertyCB(const PropertyData * pdc)
    {
        N_assert(mStyle, "error logic");
        StylePropertyCmd * prop;
        String help;
        bool redraw, layout;
        Nui32 eid;
        Nui32 name = 0;
        pdc->get(IDProperty, eid);
        pdc->get(HelpStringAttribute, help, _T("Style custom property definition - gets/sets a named user string."));
        pdc->get(RedrawOnWriteAttribute, redraw, false);
        pdc->get(LayoutOnWriteAttribute, layout, false);
        pdc->get(FireEventAttribute, eid);
        typedef std::pair<NIIf, NIIf> Range;

        prop = N_new StylePropertyCmd(name, help, redraw, layout, eid);

        N_Only(Log).log(_I18n("-----> Adding PropertyDefiniton. Name: ") + N_conv(name));

        mStyle->add(prop);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcListenerPropertyCB(const PropertyData * pdc)
    {
        N_assert(mStyle, "error logic");
        N_assert(!mListenerProperty, "error logic");

        bool redraw, layout;
        Nui32 wid, target, pid, eid;
        pdc->get(WidgetAttribute, wid);
        pdc->get(TargetPropertyAttribute, target);
        pdc->get(IDProperty, pid);
        pdc->get(RedrawOnWriteAttribute, redraw, false);
        pdc->get(LayoutOnWriteAttribute, layout, false);
        pdc->get(FireEventAttribute, eid, 0);
        typedef std::pair<NIIf, NIIf> Range;

        mListenerProperty = N_new ListenerProperty(pid, wid, target, redraw, layout, eid);

        N_Only(Log).log(_I18n("-----> Adding PropertyLinkDefiniton. Name: ") + pid);

        if((wid != 0) || (target != 0))
        {
            N_Only(Log).log(_I18n("-------> Adding link target to property: ") + 
                N_conv(target) + _I18n(" on widget: ") + N_conv(wid));
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcSizeOpCB(const PropertyData * pdc)
    {
        SizeOpType base;
        UI::StrConv::conv(pdc->get(TypeProperty), base);

        SizeBase * cloned = N_new OperatorSize(base);
        mSizeList.push_back(cloned);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcVLayoutIDCB(const PropertyData * pdc)
    {
        if(mFrameCom)
        {
            FrameComType what;
            String temp;
            VLayout fmt;
            pdc->get(TypeProperty, temp, _T("Bg"));
            UI::StrConv::conv(temp, what);
            UI::StrConv::conv(pdc->get(TypeProperty), fmt);

            switch(what)
            {
            case FCT_Left:
                mFrameCom->setLeftLayout(fmt);
                break;
            case FCT_Right:
                mFrameCom->setRightLayout(fmt);
                break;
            case FCT_Bg:
                mFrameCom->setBgVLayout(fmt);
                break;
            default:
                N_EXCEPT(InvalidRequest, _I18n("VLayoutIDUnitProperty within ") +
                    N_conv(SectionFrameUnitProperty) + _I18n(" may only be used for ")
                    _I18n("LeftEdge, RightEdge or Background components. Received: ") +
                    pdc->get(TypeProperty));
            }
        }
        else if (mImageryCom)
        {
            Nui32 id;
            pdc->get(IDProperty, id);
            mImageryCom->setVLayoutID(id);
        }
        else if (mTextCom)
        {
            Nui32 id;
            pdc->get(IDProperty, id);
            mTextCom->setVLayoutID(id);
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcHLayoutIDCB(const PropertyData * pdc)
    {
        if(mFrameCom)
        {
            FrameComType what;
            HLayout fmt;
            UI::StrConv::conv(pdc->getDefault(TypeProperty, _T("Bg")), what);
            UI::StrConv::conv(pdc->get(TypeProperty), fmt);

            switch(what)
            {
            case FCT_Top:
                mFrameCom->setTopLayout(fmt);
                break;
            case FCT_Bottom:
                mFrameCom->setBottomLayout(fmt);
                break;
            case FCT_Bg:
                mFrameCom->setBgHLayout(fmt);
                break;
            default:
                N_EXCEPT(InvalidRequest, _I18n("HLayoutIDUnitProperty within ") +
                    N_conv(SectionFrameUnitProperty) + _I18n(" may only be used for ")
                    _I18n("TopEdge, BottomEdge or Background components. Received: ") +
                    pdc->get(TypeProperty));
            }
        }
        else if (mImageryCom)
        {
            Nui32 pid;
            pdc->get(IDProperty, pid);
            mImageryCom->setHLayoutID(pid);
        }
        else if (mTextCom)
        {
            Nui32 pid;
            pdc->get(IDProperty, pid);
            mTextCom->setHLayoutID(pid);
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcComAreaIDCB(const PropertyData * pdc)
    {
        N_assert (mComArea != 0, "error logic");
        Nui32 pid;
        pdc->get(IDProperty, pid);
        mComArea->setPropertyFetch(pid);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcPixelIDCB(const PropertyData * pdc)
    {
        N_assert(mImageryCom != 0 || mFrameCom != 0, "error logic");

        if(mImageryCom)
        {
            Nui32 id;
            pdc->get(IDProperty, id);
            mImageryCom->setImageSrc(id);

            N_Only(Log).log(_I18n("---------> Using image via property: ") +
                pdc->get(NameProperty));
        }
        else if (mFrameCom)
        {
            Nui32 id;
            pdc->get(IDProperty, id);
            FrameComType temp;
            UI::StrConv::conv(pdc->get(TypeProperty), temp);
            mFrameCom->setImageSrc(temp, id);

            N_Only(Log).log(_I18n("---------> Using image via property: ") +
                pdc->get(NameProperty) + _T(" for: ") + pdc->get(TypeProperty));
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcTextIDCB(const PropertyData * pdc)
    {
        N_assert(mTextCom != 0, "error logic");

        Nui32 id;
        pdc->get(IDProperty, id);
        mTextCom->setTextID(id);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcFontIDCB(const PropertyData * pdc)
    {
        N_assert(mTextCom != 0, "error logic");

        Nui32 id;
        pdc->get(IDProperty, id);
        mTextCom->setFontID(id);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcColourCB(const PropertyData * pdc)
    {
        String temp = pdc->get(ValueProperty);
        ColourArea cols(hexStringToARGB(temp));
        assignColours(cols);
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleUnitECB()
    {
        N_Only(Log).log(_I18n("===== Look and feel parsing completed ====="));
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcWidgetStyleECB()
    {
        if(mStyle)
        {
            N_Only(Log).log(_I18n("---< End of definition for widget widget_style '") +
                N_conv(mStyle->getID()) + _T("'."));
            mStyleManager->addStyle(mStyle);
            N_delete mStyle;
            mStyle = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleWidgetECB()
    {
        N_assert(mStyle != 0, "error logic");

        if(mStyleWidget)
        {
            N_Only(Log).log(_I18n("-----< End of definition for child widget. Type: ") +
                N_conv(mStyleWidget->getViewID()) + _T("."));
            mStyle->add(*mStyleWidget);
            N_delete mStyleWidget;
            mStyleWidget = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleSectionECB()
    {
        N_assert(mStyle != 0, "error logic");

        if(mStyleSection)
        {
            N_Only(Log).log(_I18n("-----< End of definition for imagery section '") +
                N_conv(mStyleSection->getID()) + _T("'."));
            mStyle->add(*mStyleSection);
            N_delete mStyleSection;
            mStyleSection = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleLayoutECB()
    {
        N_assert(mStyle != 0, "error logic");

        if(mStyleLayout)
        {
            N_Only(Log).log(_I18n("-----< End of definition for imagery for state '") +
                N_conv(mStyleLayout->getID()) + _T("'."));
            mStyle->add(*mStyleLayout);
            N_delete mStyleLayout;
            mStyleLayout = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcLayerECB()
    {
        N_assert(mStyleLayout != 0, "error logic");

        if(mLayer)
        {
            N_Only(Log).log(_I18n("-------< End of definition of imagery layer."));
            mStyleLayout->add(*mLayer);
            N_delete mLayer;
            mLayer = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcSectionECB()
    {
        N_assert(mLayer != 0, "error logic");

        if(mSection)
        {
            mLayer->add(*mSection);
            N_delete mSection;
            mSection = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcImageryComECB()
    {
        N_assert(mStyleSection != 0, "error logic");

        if(mImageryCom)
        {
            mStyleSection->add(mImageryCom);
            mImageryCom = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcTextComECB()
    {
        N_assert(mStyleSection != 0, "error logic");

        if(mTextCom)
        {
            mStyleSection->add(mTextCom);
            mTextCom = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcFrameComECB()
    {
        N_assert(mStyleSection != 0, "error logic");

        if(mFrameCom)
        {
            mStyleSection->add(mFrameCom);
            mFrameCom = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcComAreaECB()
    {
        N_assert((mStyleWidget != 0) || (mImageryCom != 0) || (mTextCom != 0) ||
            mStyleSpecial != 0 || mFrameCom != 0, "error logic");
        N_assert(mComArea != 0, "error logic");

        if(mStyleWidget)
        {
            mStyleWidget->setArea(*mComArea);
        }
        else if(mFrameCom)
        {
            mFrameCom->setArea(*mComArea);
        }
        else if(mImageryCom)
        {
            mImageryCom->setArea(*mComArea);
        }
        else if(mTextCom)
        {
            mTextCom->setArea(*mComArea);
        }
        else if(mStyleSpecial)
        {
            mStyleSpecial->setArea(*mComArea);
        }

        N_delete mComArea;
        mComArea = 0;
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleSpecialECB()
    {
        N_assert(mStyle != 0, "error logic");

        if(mStyleSpecial)
        {
            mStyle->add(*mStyleSpecial);
            N_delete mStyleSpecial;
            mStyleSpecial = 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcAnySizeECB()
    {
        if(!mSizeList.empty())
        {
            SizeBase * currDim = mSizeList.back();
            mSizeList.pop_back();

            if(mSizeList.empty())
            {
                mSizeDefine.setValue(currDim, false);
                if(mComArea)
                {
                    switch(mSizeDefine.getType())
                    {
                    case ST_LeftPadding:
                    case ST_X:
                        mComArea->mLeft = mSizeDefine;
                        break;
                    case ST_TopPadding:
                    case ST_Y:
                        mComArea->mTop = mSizeDefine;
                        break;
                    case ST_RightPadding:
                    case ST_Width:
                        mComArea->mXDim = mSizeDefine;
                        break;
                    case ST_BottomPadding:
                    case ST_Height:
                        mComArea->mYDim = mSizeDefine;
                        break;
                    default:
                        N_EXCEPT(InvalidRequest, _I18n("Invalid SizeType specified for area component."));
                    }
                }
            }
            else
            {
                if (OperatorSize * op = dynamic_cast<OperatorSize *>(mSizeList.back()))
                    op->setNextOperand(currDim, false);
                else
                {
                    N_delete currDim;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcListenerPropertyECB()
    {
        N_assert(mListenerProperty, "error logic");
        mStyle->add(mListenerProperty);

        N_Only(Log).log(_I18n("<----- End of PropertyLinkDefiniton. Name: ") +
            mListenerProperty->getID());
        mListenerProperty = 0;
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcListenerTargetCB(const PropertyData * pdc)
    {
        N_assert(mListenerProperty, "error logic");

        Nui32 w, p;
        pdc->get(WidgetAttribute, w);
        pdc->get(PropertyAttribute, p);

        if(w != 0 || p != 0)
        {
            typedef std::pair<NIIf, NIIf> Range;

            mListenerProperty->add(w, p);

            N_Only(Log).log(_I18n("-------> Adding link target to property: ") +
                N_conv(p) + _I18n(" on widget: ") + N_conv(w));
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcAnimationCB(const PropertyData * pdc)
    {
        N_assert(mStyle != 0, "error logic");

        mTag = N_new AnimationXmlAnalyze(mStyle, mLangMap->getLangID());
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcListenerEventCB(const PropertyData * pdc)
    {
        N_assert(mStyle, "error logic");
        N_assert(mListenerEvent == 0, "error logic");

        Nui32 wid, eid;
        pdc->get(WidgetAttribute, wid);
        pdc->get(EventAttribute, eid);

        Nui32 tid;
        pdc->get(IDProperty, tid);
        mListenerEvent = N_new ListenerEvent(tid);

        N_Only(Log).log(_I18n("-----> Adding EventLinkDefiniton. Name: ") +
            mListenerEvent->getID());

        N_assert(mListenerEvent, "error logic");

        if(wid !=0 || eid != 0)
        {
            mListenerEvent->add(wid, eid);
            N_Only(Log).log(_I18n("-------> Adding link target to event: ") + 
                N_conv(eid) + _I18n(" on widget: ") + N_conv(wid));
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcEventTargetCB(const PropertyData * pdc)
    {
        Nui32 wid, eid;
        pdc->get(WidgetAttribute, wid);
        pdc->get(EventAttribute, eid);

        N_assert(mListenerEvent, "error logic");

        if(wid != 0 || eid != 0)
        {
            mListenerEvent->add(wid, eid);
            N_Only(Log).log(_I18n("-------> Adding link target to event: ") + 
                N_conv(eid) + _I18n(" on widget: ") + N_conv(wid));
        }
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcListenerEventECB()
    {
        N_assert(mListenerEvent, "error logic");
        mStyle->add(*mListenerEvent);

        N_Only(Log).log(_I18n("<----- End of EventLinkDefiniton. Name: ") +
            mListenerEvent->getID());

        N_delete mListenerEvent;
        mListenerEvent = 0;
    }
    //------------------------------------------------------------------------
    void StyleXmlAnalyze::PrcStyleSpecialIDCB(const PropertyData * pdc)
    {
        N_assert(mComArea != 0, "error logic");

        Nui32 wstyle, id;
        pdc->get(WidgetStyleAttribute, wstyle);
        pdc->get(IDProperty, id);

        mComArea->setSrcFetch(wstyle == 0 ? mStyle->getID() : wstyle, id);
    }
    //------------------------------------------------------------------------
}
}