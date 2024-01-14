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
#include "NiiUIStyleSpecial.h"
#include "NiiUIStyleXmlAnalyze.h"
#include "NiiUIWidgetManager.h"
#include "NiiXmlSerializer.h"
#include "NiiFontManager.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StylePropertyCmd
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    void StylePropertyCmd::write(XmlSerializer * out) const
    {
        out->begin(_T("PropertyDefine"));

        out->attribute(_T("name"), mID);

        String value;
        getDefault(value);
        if(!value.empty())
            out->attribute(_T("value"), value);

        if(!getVerbose().empty())
            out->attribute(_T("verbose"), getVerbose());

        if(mRefresh)
            out->attribute(_T("signal_refresh"), _T("true"));

        if(mLayout)
            out->attribute(_T("signal_layout"), _T("true"));

        if(mSetEvent != 0)
            out->attribute(_T("signal_event"), mSetEvent);

        out->end();
    }
    //------------------------------------------------------------------------
    bool StylePropertyCmd::isSerialize() const
    {
        return true;
    }
    //------------------------------------------------------------------------
    void StylePropertyCmd::init(PropertyCmdObj * obj) const
    {
        String out;
        getDefault(out);
        static_cast<Widget *>(obj)->setExtData(mCustomDataID, out);
    }
    //------------------------------------------------------------------------
    void StylePropertyCmd::getDefault(String & out) const
    {

    }
    //------------------------------------------------------------------------
    PropertyCmd * StylePropertyCmd::clone() const
    {
        return N_new StylePropertyCmd(*this);
    }
    //------------------------------------------------------------------------
    void StylePropertyCmd::get(const PropertyCmdObj * obj, String & out) const
    {
        const Widget * widget = static_cast<const Widget *>(obj);

        // the try/catch is used instead of querying the existence of the user
        // string in order that for the 'usual' case - where the user string
        // exists - there is basically no additional overhead, and that any
        // overhead is incurred only for the initial creation of the user
        // string.
        // Maybe the only negative here is that an error gets logged, though
        // this can be treated as a 'soft' error.
        try
        {
            widget->getExtData(mCustomDataID, out);
        }
        catch(UnknownObjectException &)
        {
            N_Only(Log).log(_I18n("Defining new user string: ") + N_conv(mCustomDataID));

            getDefault(out);

            // HACK: FIXME: TODO: This const_cast is basically to allow the
            // above mentioned performance measure; the alternative would be
            // to just return d_default, and while technically correct, it
            // would be very slow.
            const_cast<Widget *>(widget)->setExtData(mCustomDataID, out);
        }
    }
    //------------------------------------------------------------------------
    void StylePropertyCmd::set(PropertyCmdObj * obj, const String & value)
    {
        Widget * widget = static_cast<Widget *>(obj);
        widget->setExtData(mCustomDataID, value);

        if(mLayout)
            widget->layout();

        if(mRefresh)
            widget->refresh();

        if(mSetEvent != 0)
        {
            WidgetEventArgs args(widget);
            args.mWidget->signal(mSetEvent, &args);
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleArea::StyleArea() :
        mLeft(AbsoluteSize(0.0f), ST_LeftPadding),
        mTop(AbsoluteSize(0.0f), ST_TopPadding),
        mXDim(RelationSize(RelPosf(1.0f, 0.0f), ST_Width), ST_RightPadding),
        mYDim(RelationSize(RelPosf(1.0f, 0.0f), ST_Height), ST_BottomPadding)
    {
    }
    //------------------------------------------------------------------------
    Rectf StyleArea::getPixelRect(const Widget & wnd) const
    {
        Rectf pixelRect;

        // use a property?
        if(isPropertyFetch())
        {
            RelRectf out;
            wnd.get(mPropertyID, &out);
            pixelRect = abs(out, wnd.getPixelSize());
        }
        else if(isSpecialFetch())
        {
            return N_Only(Widget).getStyle(mStyleID)->getSpecial(mPropertyID).getArea().getPixelRect(wnd);
        }
        // not via property or named area- calculate using Dimensions
        else
        {
            // sanity check, we mus be able to form a Rect from what we represent.
            N_assert(mLeft.getType() == ST_LeftPadding || mLeft.getType() == ST_X, "error");
            N_assert(mTop.getType() == ST_TopPadding || mTop.getType() == ST_Y, "error");
            N_assert(mXDim.getType() == ST_RightPadding || mXDim.getType() == ST_Width, "error");
            N_assert(mYDim.getType() == ST_BottomPadding || mYDim.getType() == ST_Height, "error");

            pixelRect.setLeft(mLeft.getValue()->getValue(wnd));
            pixelRect.setTop(mTop.getValue()->getValue(wnd));

            if(mXDim.getType() == ST_Width)
                pixelRect.setWidth(mXDim.getValue()->getValue(wnd));
            else
                pixelRect.setRight(mXDim.getValue()->getValue(wnd));

            if(mYDim.getType() == ST_Height)
                pixelRect.setHeight(mYDim.getValue()->getValue(wnd));
            else
                pixelRect.setBottom(mYDim.getValue()->getValue(wnd));
        }

        return pixelRect;
    }
    //------------------------------------------------------------------------
    Rectf StyleArea::getPixelRect(const Widget & wnd, const Rectf & container) const
    {
        Rectf pixelRect;

        if(isPropertyFetch())
        {
            RelRectf out;
            wnd.get(mPropertyID, &out);
            pixelRect = abs(out, wnd.getPixelSize());
        }
        else if(isSpecialFetch())
        {
            return N_Only(Widget).getStyle(mStyleID)->
                getSpecial(mPropertyID).getArea().getPixelRect(wnd, container);
        }
        // not via property or named area- calculate using Dimensions
        else
        {
            // sanity check, we mus be able to form a Rect from what we represent.
            N_assert(mLeft.getType() == ST_LeftPadding || mLeft.getType() == ST_X, "error");
            N_assert(mTop.getType() == ST_TopPadding || mTop.getType() == ST_Y, "error");
            N_assert(mXDim.getType() == ST_RightPadding || mXDim.getType() == ST_Width, "error");
            N_assert(mYDim.getType() == ST_BottomPadding || mYDim.getType() == ST_Height, "error");

            pixelRect.setLeft(mLeft.getValue()->getValue(wnd, container) + container.left());
            pixelRect.setTop(mTop.getValue()->getValue(wnd, container) + container.top());

            if(mXDim.getType() == ST_Width)
                pixelRect.setWidth(mXDim.getValue()->getValue(wnd, container));
            else
                pixelRect.setRight(mXDim.getValue()->getValue(wnd, container) + container.left());

            if(mYDim.getType() == ST_Height)
                pixelRect.setHeight(mYDim.getValue()->getValue(wnd, container));
            else
                pixelRect.setBottom(mYDim.getValue()->getValue(wnd, container) + container.top());
        }

        return pixelRect;
    }
    //------------------------------------------------------------------------
    void StyleArea::write(XmlSerializer * out) const
    {
        out->begin(_T("Area"));

        if(isPropertyFetch())
        {
            out->begin(_T("AreaProperty")).attribute(_T("name"), mPropertyID).end();
        }
        else if(isSpecialFetch())
        {
            out->begin(_T("NamedAreaSource"))
                .attribute(_T("look"), mStyleID)
                .attribute(_T("name"), mPropertyID)
                .end();
        }
        // not a property, write out individual dimensions explicitly.
        else
        {
            mLeft.write(out);
            mTop.write(out);
            mXDim.write(out);
            mYDim.write(out);
        }
        out->end();
    }
    //------------------------------------------------------------------------
    void StyleArea::setPropertyFetch(PropertyID pid)
    {
        mPropertyID = pid;
        mStyleID = 0;
    }
    //------------------------------------------------------------------------
    void StyleArea::setSrcFetch(StyleID widget_style, Nid style_special)
    {
        mStyleID = widget_style;
        mPropertyID = style_special;
    }
    //------------------------------------------------------------------------
    bool StyleArea::notifyFont(Widget * window, const Font * font) const
    {
        if(isPropertyFetch())
            return false;

        if(isSpecialFetch())
        {
            N_Only(Widget).getStyle(mStyleID)->getSpecial(mPropertyID).notifyFont(window, font);
        }
        bool result = 0;
        const FontSize * fs = 0;
        if (mLeft.getValue())
        {
            fs = dynamic_cast<const FontSize *>(mLeft.getValue());
            result |= (font == (fs->getFont() == 0 ? window->getFont() : N_Only(Font).get(fs->getFont())));
        }
        if (mTop.getValue())
        {
            fs = dynamic_cast<const FontSize *>(mTop.getValue());
            result |= (font == (fs->getFont() == 0 ? window->getFont() : N_Only(Font).get(fs->getFont())));
        }
        if (mXDim.getValue())
        {
            fs = dynamic_cast<const FontSize *>(mXDim.getValue());
            result |= (font == (fs->getFont() == 0 ? window->getFont() : N_Only(Font).get(fs->getFont())));
        }
        if (mYDim.getValue())
        {
            fs = dynamic_cast<const FontSize *>(mYDim.getValue());
            result |= (font == (fs->getFont() == 0 ? window->getFont() : N_Only(Font).get(fs->getFont())));
        }


        //bool result = mLeft.notifyFont(widget, font);
        //result |= mTop.notifyFont(widget, font);
        //result |= mXDim.notifyFont(widget, font);
        //result |= mYDim.notifyFont(widget, font);

        return result;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleSpecial
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleSpecial::StyleSpecial() 
    {
    }
    //------------------------------------------------------------------------
    StyleSpecial::StyleSpecial(Nid id) :
        mID(id)
    {
    }
    //------------------------------------------------------------------------
    void StyleSpecial::write(XmlSerializer * out) const
    {
        out->begin(_T("Special")).attribute(_T("name"), mID);
        mArea.write(out);
        out->end();
    }
    //------------------------------------------------------------------------
    bool StyleSpecial::notifyFont(Widget * obj, const Font * font) const
    {
        return false;
    }
    //------------------------------------------------------------------------
}
}