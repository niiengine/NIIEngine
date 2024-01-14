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
#include "NiiUIStyleSection.h"
#include "NiiUIException.h"
#include "NiiUIStrConv.h"
#include "NiiUIPixel.h"
#include "NiiColour.h"
#include "NiiXmlSerializer.h"
#include "NiiFontManager.h"
#include "NiiPixelManager.h"
#include "NiiGeometryPixel.h"
#include <iostream>
#include <cstdlib>
#include <limits>

#if defined(CEGUI_USE_FRIBIDI)
    #include "NiiUIFribidiText.h"
#elif defined(CEGUI_USE_MINIBIDI)
    #include "NiiUIMinibidiText.h"
#else
    #include "NiiUIBidiText.h"
#endif

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleSection
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleSection::StyleSection() :
        mColour(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
    {
    }
    //------------------------------------------------------------------------
    StyleSection::StyleSection(Nid id) :
        mID(id),
        mColour(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
    {
    }
    //------------------------------------------------------------------------
    StyleSection::~StyleSection()
    {
        clearAll();
    }
    //------------------------------------------------------------------------
    void StyleSection::render(const Widget * dest, const ColourArea * ca,
        const Rectf * clipper) const
    {
        // decide what to do as far as colours go
        ColourArea finalCols;

        if(mColourID != 0)
        {
            dest->get(mColourID, &finalCols);
        }
        else
        {
            finalCols = mColour;
        }

        if(ca)
            finalCols *= *ca;

        ColourArea * dstcol = (finalCols.isOneColour() && finalCols.mTL.getARGB() == 0xFFFFFFFF) ? 0 : &finalCols;

        // render all image components in this section
        UnitList::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            (*i)->render(dest, dstcol, clipper);
        }
    }
    //------------------------------------------------------------------------
    void StyleSection::render(const Widget * dest, const Rectf & rect, const ColourArea * ca,
        const Rectf * clipper) const
    {
        // decide what to do as far as colours go
        ColourArea finalCols;

        if(mColourID != 0)
        {
            dest->get(mColourID, &finalCols);
        }
        else
        {
            finalCols = mColour;
        }

        if(ca)
            finalCols *= *ca;

        ColourArea * dstcol = (finalCols.isOneColour() && finalCols.mTL.getARGB() == 0xFFFFFFFF) ? 0 : &finalCols;

        // render all image components in this section
        UnitList::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            (*i)->render(dest, rect, dstcol, clipper);
        }
    }
    //------------------------------------------------------------------------
    void StyleSection::add(StyleSectionUnit * unit)
    {
        mUnitList.push_back(unit);
    }
    //------------------------------------------------------------------------
    void StyleSection::remove(StyleSectionUnit * unit)
    {
        UnitList::iterator f, fend = mUnitList.end();
        for(f = mUnitList.begin(); f != fend; ++f)
        {
            if ((*f)->equal(*unit))
            {
                N_delete *f;
                mUnitList.erase(f);
                break;
            }
        }
    }
    //------------------------------------------------------------------------
    void StyleSection::clearAll()
    {
        UnitList::iterator f, fend = mUnitList.end();
        for (f = mUnitList.begin(); f != fend; ++f)
        {
            N_delete *f;
        }
        mUnitList.clear();
    }
    //------------------------------------------------------------------------
    Rectf StyleSection::getBoundingRect(const Widget & e) const
    {
        Rectf compRect;
        Rectf bounds(std::numeric_limits<NIIf>::max(), std::numeric_limits<NIIf>::max(),
            std::numeric_limits<NIIf>::min(), std::numeric_limits<NIIf>::min());

        UnitList::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            compRect = (*i)->getArea().getPixelRect(e);

            bounds.setLeft(N_MIN(NIIf, bounds.left(), compRect.left()));
            bounds.setTop(N_MIN(NIIf, bounds.top(), compRect.top()));
            bounds.setRight(N_MAX(NIIf, bounds.right(), compRect.right()));
            bounds.setBottom(N_MAX(NIIf, bounds.bottom(), compRect.bottom()));
        }

        return bounds;
    }
    //------------------------------------------------------------------------
    Rectf StyleSection::getBoundingRect(const Widget & wnd, const Rectf & rect) const
    {
        Rectf compRect;
        Rectf bounds(std::numeric_limits<NIIf>::max(), std::numeric_limits<NIIf>::max(),
            std::numeric_limits<NIIf>::min(), std::numeric_limits<NIIf>::min());

        UnitList::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            compRect = (*i)->getArea().getPixelRect(wnd, rect);

            bounds.setLeft(N_MIN(NIIf, bounds.left(), compRect.left()));
            bounds.setTop(N_MIN(NIIf, bounds.top(), compRect.top()));
            bounds.setRight(N_MAX(NIIf, bounds.right(), compRect.right()));
            bounds.setBottom(N_MAX(NIIf, bounds.bottom(), compRect.bottom()));
        }

        return bounds;
    }
    //------------------------------------------------------------------------
    void StyleSection::write(XmlSerializer * xml_stream) const
    {
        // output opening tag
        xml_stream->begin(_T("StyleSection")).attribute(_T("id"), mID);

        // output modulative colours for this section
        if(mColourID != 0)
        {
            xml_stream->begin(_T("ColourAreaAttribute"));
            xml_stream->attribute(_T("id"), mColourID).end();
        }
        else if(!mColour.isOneColour() || mColour.mTL != Colour(1,1,1,1))
        {
            xml_stream->begin(_T("Colours"))
                .attribute(_T("top_left"), N_conv(mColour.mTL))
                .attribute(_T("top_right"), N_conv(mColour.mTR))
                .attribute(_T("bottom_left"), N_conv(mColour.mBL))
                .attribute(_T("bottom_right"), N_conv(mColour.mBR))
                .end();
        }

        UnitList::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            (*i)->writeColour(xml_stream);
        }

        // output closing tag
        xml_stream->end();
    }
    //------------------------------------------------------------------------
    bool StyleSection::notifyFont(Widget * window, const Font * font) const
    {
        bool result = false;
        UnitList::const_iterator i, iend = mUnitList.end();
        for(i = mUnitList.begin(); i != iend; ++i)
        {
            result |= (*i)->notifyFont(window, font);
        }

        return result;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleSectionUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleSectionUnit::StyleSectionUnit() :
        mColour(0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF)
    {
    }
    //------------------------------------------------------------------------
    StyleSectionUnit::~StyleSectionUnit()
    {
    }
    //------------------------------------------------------------------------
    void StyleSectionUnit::render(const Widget * dest, const ColourArea * colour, const Rectf * clip) const
    {
        Rectf drect(mArea.getPixelRect(*dest));

        if(!clip)
            clip = &drect;

        const Rectf frect(drect.intersects(*clip));
        renderImpl(dest, drect, colour, &frect);
    }
    //------------------------------------------------------------------------
    void StyleSectionUnit::render(const Widget * dest, const Rectf & area, const ColourArea * colour,
        const Rectf * clip) const
    {
        Rectf drect(mArea.getPixelRect(*dest, area));

        if(!clip)
            clip = &drect;

        const Rectf frect(drect.intersects(*clip));
        renderImpl(dest, drect, colour, &frect);
    }
    //------------------------------------------------------------------------
    void StyleSectionUnit::getColourImpl(const Widget * dest, const ColourArea * colour, ColourArea & out) const
    {
        if(mColourID != 0)
        {
            dest->get(mColourID, &out);
        }
        else
        {
            out = mColour;
        }
        if (colour)
        {
            out *= *colour;
        }
    }
	//------------------------------------------------------------------------
	void StyleSectionUnit::write(XmlSerializer *) const
	{

	}
    //------------------------------------------------------------------------
    bool StyleSectionUnit::writeColour(XmlSerializer * out) const
    {
        if(mColourID != 0)
        {
            out->begin(_T("ColourAreaAttribute"));
            out->attribute(_T("name"), mColourID).end();
        }
        else if(!mColour.isOneColour() || mColour.mTL != Colour(1, 1, 1, 1))
        {
            out->begin(_T("Colours"))
                .attribute(_T("top_left"), N_conv(mColour.mTL))
                .attribute(_T("top_right"), N_conv(mColour.mTR))
                .attribute(_T("bottom_left"), N_conv(mColour.mBL))
                .attribute(_T("bottom_right"), N_conv(mColour.mBR))
                .end();
        }
        else
            return false;
        return true;
    }
    //------------------------------------------------------------------------
    bool StyleSectionUnit::notifyFont(Widget * e, const Font * font) const
    {
        return false;
    }
    //------------------------------------------------------------------------
    bool StyleSectionUnit::equal(const StyleSectionUnit &) const
    {
        return false;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleSectionTextUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleSectionTextUnit::StyleSectionTextUnit() :
#ifndef CEGUI_BIDI_SUPPORT
        mBidiText(0),
#elif defined (CEGUI_USE_FRIBIDI)
        mBidiText(N_new FribidiText),
#elif defined (CEGUI_USE_MINIBIDI)
        mBidiText(N_new MinibidiText),
#else
    #error "BIDI Configuration is inconsistant, check your config!"
#endif
        mValidBidi(false),
        mLayoutText(N_new LeftLayout(&mRenderText)),
        mLastHLayout(HTL_Left),
        mVLayout(VTL_Top),
        mHLayout(HTL_Left){}
    //------------------------------------------------------------------------
    StyleSectionTextUnit::~StyleSectionTextUnit()
    {
        N_delete mBidiText;
    }
    //------------------------------------------------------------------------
    StyleSectionTextUnit::StyleSectionTextUnit(const StyleSectionTextUnit & obj) :
        StyleSectionUnit(obj),
        mText(obj.mText),
#ifndef CEGUI_BIDI_SUPPORT
        mBidiText(0),
#elif defined (CEGUI_USE_FRIBIDI)
        mBidiText(N_new FribidiText),
#elif defined (CEGUI_USE_MINIBIDI)
        mBidiText(N_new MinibidiText),
#endif
        mValidBidi(false),
        mRenderText(obj.mRenderText),
        mLayoutText(obj.mLayoutText),
        mLastHLayout(obj.mLastHLayout),
        mFont(obj.mFont),
        mVLayout(obj.mVLayout),
        mHLayout(obj.mHLayout),
        mTextID(obj.mTextID),
        mFontID(obj.mFontID){}
    //------------------------------------------------------------------------
    StyleSectionTextUnit& StyleSectionTextUnit::operator=(const StyleSectionTextUnit & o)
    {
        if (this == &o)
            return *this;

        StyleSectionUnit::operator=(o);

        mText = o.mText;
        // note we do not assign the BidiText object, we just mark our
        // existing one as invalid so it's data gets regenerated next time it's
        // needed.
        mValidBidi = false;
        mRenderText = o.mRenderText;
        mLayoutText = o.mLayoutText;
        mLastHLayout = o.mLastHLayout;
        mFont = o.mFont;
        mVLayout = o.mVLayout;
        mHLayout = o.mHLayout;
        mTextID = o.mTextID;
        mFontID = o.mFontID;

        return *this;
    }
    //------------------------------------------------------------------------
    void StyleSectionTextUnit::setupLayout(const Widget * window, const PixelUnitGrid * text) const
    {
        HTextLayout horzf = mHLayout.get(window);
        if(horzf == mLastHLayout)
        {
            mLayoutText->setGrid(text);
            return;
        }

        mLastHLayout = horzf;

        switch(horzf)
        {
        case HTL_Left:
            mLayoutText.bind(N_new LeftLayout(text));
            break;
        case HTL_Centre:
            mLayoutText.bind(N_new CenterLayout(text));
            break;
        case HTL_Right:
            mLayoutText.bind(N_new RightLayout(text));
            break;
        case HTL_Normal:
            mLayoutText.bind(N_new JustifyLayout(text));
            break;
        case HTL_Wrap_Left:
            mLayoutText.bind(N_new WrapLayout<LeftLayout>(text));
            break;
        case HTL_Wrap_Centre:
            mLayoutText.bind(N_new WrapLayout<CenterLayout>(text));
            break;
        case HTL_Wrap_Right:
            mLayoutText.bind(N_new WrapLayout<RightLayout>(text));
            break;
        case HTL_Wrap_Expand:
            mLayoutText.bind(N_new WrapLayout<JustifyLayout>(text));
            break;
        }
    }
    //------------------------------------------------------------------------
    bool StyleSectionTextUnit::equal(const StyleSectionUnit & o) const
    {
        const StyleSectionTextUnit * dst = dynamic_cast<const StyleSectionTextUnit *>(&o);
        if(dst)
            return (mText == dst->mText && mFont == dst->mFont);
        return false;
    }
    //------------------------------------------------------------------------
    void StyleSectionTextUnit::renderImpl(const Widget * dest, const Rectf & dRect,
        const ColourArea * mc, const Rectf * clipper) const
    {
        const Font * font = getFont(dest);
        Rectf destRect = dRect;
        // exit if we have no font to use.
        if(!font)
            return;

        const PixelUnitGrid * rs = &mRenderText;
        // do we fetch text from a property
        if(mTextID != 0)
        {
            // fetch text & do bi-directional reordering as needed
            String vis;
            String temp;
#ifdef CEGUI_BIDI_SUPPORT
            dest->get(mTextID, temp);
            mBidiText->reorder(temp, vis);
#else
            dest->get(mTextID, vis);
#endif
            // parse string using parser from Widget.
            mRenderText = dest->getRenderTextView()->parse(vis, font, 0);
        }
        // do we use a static text string from the looknfeel
        else if(!getVisualText().empty())
        {
            // parse string using parser from Widget.
            mRenderText = dest->getRenderTextView()->parse(getVisualText(), font, 0);
        }
        // do we have to override the font?
        else if(font != dest->getFont())
        {
            mRenderText = dest->getRenderTextView()->parse(dest->getVisualText(), font, 0);
        }
        // use ready-made PixelUnitGrid from the Widget itself
        else
        {
            rs = dest->getRenderText();
        }

        setupLayout(dest, rs);
        mLayoutText->layout(dest, destRect.getSize());

        // Get total formatted height.
        NIIf textHeight = mLayoutText->getVExtent(dest);

        // handle dest area adjustments for vertical formatting.
        VTextLayout vertFormatting = mVLayout.get(dest);

        switch(vertFormatting)
        {
        case VTL_Centre:
            destRect.mTop += (destRect.getHeight() - textHeight) * 0.5f;
            break;
        case VTL_Bottom:
            destRect.mTop = destRect.mBottom - textHeight;
            break;
        default:
            // default is VTL_Top, for which we take no action.
            break;
        }

        // calculate final colours to be used
        ColourArea finalColours;
        getColourImpl(dest, mc, finalColours);

        // add geometry for text to the target window.
        mLayoutText->draw(dest, *dest->getPixelData(), destRect.getPos(), &finalColours, clipper);
    }
    //------------------------------------------------------------------------
    const Font * StyleSectionTextUnit::getFont(const Widget * e) const
    {
        try
        {
            if(mFontID == 0)
            {
                if(mFont == 0)
                {
                    return e->getFont();
                }
                else
                {
                    return N_Only(Font).getFont(mFont);
                }
            }
            else
            {
                Nid temp;
                e->get(mFontID, &temp);
                return N_Only(Font).getFont(temp);
            }
        }
        catch(UnknownObjectException &)
        {
            return 0;
        }
    }
    //------------------------------------------------------------------------
    void StyleSectionTextUnit::write(XmlSerializer * dest) const
    {
        dest->begin(_T("StyleSectionTextUnit"));
        // write out area
        mArea.write(dest);

        // write text element
        if(mFont != 0 && !getText().empty())
        {
            dest->begin(_T("Text"));
            if(mFont != 0)
                dest->attribute(_T("font"), N_conv(mFont));
            if(!getText().empty())
                dest->attribute(_T("string"), getText());
            dest->end();
        }

        if(mTextID != 0)
        {
            dest->begin(_T("TextProperty")).attribute(_T("name"), N_conv(mTextID)).end();
        }

        if(mFontID != 0)
        {
            dest->begin(_T("FontProperty")).attribute(_T("name"), N_conv(mFontID)).end();
        }

        writeColour(dest);

        mVLayout.write(dest);
        mHLayout.write(dest);

        dest->end();
    }
    //------------------------------------------------------------------------
    const String & StyleSectionTextUnit::getVisualText() const
    {
        if(!mBidiText)
            return mText;

        if(!mValidBidi)
        {
            mBidiText->setSrc(mText);
            mValidBidi = true;
        }
        return mBidiText->getDst();
    }
    //------------------------------------------------------------------------
    NIIf StyleSectionTextUnit::getHTextExtent(const Widget & e) const
    {
        return mLayoutText->getHExtent(&e);
    }
    //------------------------------------------------------------------------
    NIIf StyleSectionTextUnit::getVTextExtent(const Widget & e) const
    {
        return mLayoutText->getVExtent(&e);
    }
    //------------------------------------------------------------------------
    bool StyleSectionTextUnit::notifyFont(Widget * e, const Font * font) const
    {
        bool res = StyleSectionUnit::notifyFont(e, font);

        if(font == getFont(e))
        {
            e->refresh();
            return true;
        }
        return res;
    }
    //------------------------------------------------------------------------
    String StyleSectionTextUnit::getDestText(const Widget & e) const
    {
        if(mTextID != 0)
        {
            String temp;
            e.get(mTextID, temp);
            return temp;
        }
        else if(mText.empty())
            return e.getText();
        else
            return mText;
    }
    //------------------------------------------------------------------------
    String StyleSectionTextUnit::getEffectText(const Widget & e) const
    {
#ifndef CEGUI_BIDI_SUPPORT
        return getDestText(e);
#else
        if(!mTextID.empty())
        {
            String visual;
            String temp;
            e.get(mTextID, temp);
            mBidiText->reorder(temp, visual);

            return visual;
        }
        else if(d_text.empty())
            return e.getVisualText();
        else
            getVisualText();
#endif
    }
    //------------------------------------------------------------------------
    FontID StyleSectionTextUnit::getDestFont(const Widget & e) const
    {
        if(mFontID != 0)
        {
            Nid temp;
            e.get(mFontID, &temp);
            return temp;
        }
        else if(mFont == 0)
        {
            if(const Font * font = e.getFont())
                return font->getID();
            else
                return 0;
        }
        else
            return mFont;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleSectionImageUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    StyleSectionImageUnit::StyleSectionImageUnit() :
        mImage(0),
        mVLayout(VL_Top),
        mHLayout(HL_Left)
    {
    }
    //------------------------------------------------------------------------
    void StyleSectionImageUnit::renderImpl(const Widget * dest, const Rectf & destRect,
        const ColourArea * mc, const Rectf * clipper) const
    {
        // get final image to use.
        PixelBuffer * pb = 0;
        if(mImageID != 0)
            dest->get(mImageID, pb);
        else
            pb = mImage;

        // do not draw anything if image is not set.
        if(!pb)
            return;

        HLayout horzFormatting = mHLayout.get(dest);
        VLayout vertFormatting = mVLayout.get(dest);

        Nui32 horzTiles, vertTiles;
        NIIf xpos, ypos;

        PlaneSizef imgSz(pb->getSize());

        // calculate final colours to be used
        ColourArea finalColours;
        getColourImpl(dest, mc, finalColours);

        // calculate initial x co-ordinate and horizontal tile count according to formatting options
        switch(horzFormatting)
        {
        case HL_Expand:
            imgSz.mWidth = destRect.getWidth();
            xpos = destRect.left();
            horzTiles = 1;
            break;
        case HL_Tile:
            xpos = destRect.left();
            horzTiles = std::abs(static_cast<int>(
                (destRect.getWidth() + (imgSz.mWidth - 1)) / imgSz.mWidth));
            break;
        case HL_Left:
            xpos = destRect.left();
            horzTiles = 1;
            break;
        case HL_Centre:
            xpos = destRect.left() + N_ToPixel((destRect.getWidth() - imgSz.mWidth) * 0.5f);
            horzTiles = 1;
            break;
        case HL_Right:
            xpos = destRect.right() - imgSz.mWidth;
            horzTiles = 1;
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("An unknown HLayout value was specified."));
        }

        // calculate initial y co-ordinate and vertical tile count according to formatting options
        switch (vertFormatting)
        {
        case VL_Expand:
            imgSz.mHeight = destRect.getHeight();
            ypos = destRect.top();
            vertTiles = 1;
            break;
        case VL_Tile:
            ypos = destRect.top();
            vertTiles = std::abs(static_cast<int>(
                (destRect.getHeight() + (imgSz.mHeight - 1)) / imgSz.mHeight));
            break;
        case VL_Top:
            ypos = destRect.top();
            vertTiles = 1;
            break;
        case VL_Centre:
            ypos = destRect.top() + N_ToPixel((destRect.getHeight() - imgSz.mHeight) * 0.5f);
            vertTiles = 1;
            break;
        case VL_Bottom:
            ypos = destRect.bottom() - imgSz.mHeight;
            vertTiles = 1;
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("An unknown VLayout value was specified."));
        }

        // perform final rendering (actually is now a caching of the images which will be drawn)
        Rectf finalRect;
        Rectf finalClipper;
        const Rectf * clippingRect;
        finalRect.setTop(ypos);
        finalRect.setBottom(ypos + imgSz.mHeight);

        for(NCount row = 0; row < vertTiles; ++row)
        {
            finalRect.setLeft(xpos);
            finalRect.setRight(xpos + imgSz.mWidth);

            for(NCount col = 0; col < horzTiles; ++col)
            {
                // use custom clipping for right and bottom edges when tiling the imagery
                if(((vertFormatting == VL_Tile) && row == vertTiles - 1) ||
                    ((horzFormatting == HL_Tile) && col == horzTiles - 1))
                {
                    finalClipper = clipper ? clipper->intersects(destRect) : destRect;
                    clippingRect = &finalClipper;
                }
                // not tiliing, or not on far edges, just used passed in clipper (if any).
                else
                {
                    clippingRect = clipper;
                }

                // add geometry for image to the target window.
                dest->getPixelData()->add(*pb, finalRect, clippingRect, finalColours);

                finalRect.mLeft += imgSz.mWidth;
                finalRect.mRight += imgSz.mWidth;
            }
            finalRect.mTop += imgSz.mHeight;
            finalRect.mBottom += imgSz.mHeight;
        }
    }
    //------------------------------------------------------------------------
    void StyleSectionImageUnit::write(XmlSerializer * out) const
    {
        out->begin(_T("StyleSectionImageUnit"));

        mArea.write(out);

        if(mImageID != 0)
            out->begin(_T("PixelProperty")).attribute(_T("id"), mImageID).end();
        else
            out->begin(_T("Pixel")).attribute(_T("id"), mImage->getID()).end();

        writeColour(out);

        mVLayout.write(out);
        mHLayout.write(out);

        out->end();
    }
    //------------------------------------------------------------------------
    bool StyleSectionImageUnit::equal(const StyleSectionUnit & o) const
    {
        const StyleSectionImageUnit * dst = dynamic_cast<const StyleSectionImageUnit *>(&o);
        if(dst)
            return mImage == dst->mImage;
        return false;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // StyleSectionFrameUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    struct FrameComSrc
    {
        FrameComSrc() :
            mPropertyIDValid(false),
            mImage(0){}

        inline bool operator==(const FrameComSrc & o) const
        {
            return mPropertyIDValid == o.mPropertyIDValid && mImage == o.mImage && mPropertyID == o.mPropertyID;
        }

        inline bool operator!=(const FrameComSrc & o) const
        {
            return mPropertyIDValid != o.mPropertyIDValid || mImage != o.mImage || mPropertyID != o.mPropertyID;
        }

        const PixelBuffer * mImage;
        PropertyID mPropertyID;
        bool mPropertyIDValid;
    };
    //------------------------------------------------------------------------
    StyleSectionFrameUnit::StyleSectionFrameUnit() :
        mLeftLayout(VL_Expand),
        mRightLayout(VL_Expand),
        mTopLayout(HL_Expand),
        mBottomLayout(HL_Expand),
        mBgVLayout(VL_Expand),
        mBgHLayout(HL_Expand)
    {
        mFrames = N_new_array_t(FrameComSrc, FCT_Count);
    }
    //------------------------------------------------------------------------
    StyleSectionFrameUnit::~StyleSectionFrameUnit()
    {
        N_delete_array_t(mFrames, FrameComSrc, FCT_Count);
    }
    //------------------------------------------------------------------------
    const PixelBuffer * StyleSectionFrameUnit::getImage(FrameComType part, const Widget * w) const
    {
        N_assert(part < FCT_Count, "error logic");

        if(!mFrames[part].mPropertyIDValid)
            return 0;

        if(mFrames[part].mPropertyID == 0)
            return mFrames[part].mImage;

        PixelBuffer * temp = 0;
        w->get(mFrames[part].mPropertyID, temp);
        return temp;
    }
    //------------------------------------------------------------------------
    void StyleSectionFrameUnit::setImage(FrameComType part, const PixelBuffer * pb)
    {
        N_assert(part < FCT_Count, "error logic");

        mFrames[part].mImage = pb;
        mFrames[part].mPropertyIDValid = (pb != 0);
        mFrames[part].mPropertyID = 0;
    }
    //------------------------------------------------------------------------
    void StyleSectionFrameUnit::setImage(FrameComType part, ResourceID rid, PixelBufferID pbid)
    {
        const PixelBuffer * pb;

        try
        {
            pb = N_Only(Pixel).get(rid, pbid);
        }
        catch (UnknownObjectException &)
        {
            pb = 0;
        }

        setImage(part, pb);
    }
    //------------------------------------------------------------------------
    void StyleSectionFrameUnit::setImageSrc(FrameComType part, PropertyID pid)
    {
        N_assert(part < FCT_Count, "error logic");
        mFrames[part].mImage = 0;
        mFrames[part].mPropertyIDValid = pid != 0;
        mFrames[part].mPropertyID = pid;
    }
    //------------------------------------------------------------------------
    bool StyleSectionFrameUnit::isFrameExist(FrameComType part) const
    {
        N_assert(part < FCT_Count, "error logic");
        return mFrames[part].mPropertyIDValid;
    }
    //------------------------------------------------------------------------
    bool StyleSectionFrameUnit::isFetchSrc(FrameComType part) const
    {
        N_assert(part < FCT_Count, "error logic");
        return mFrames[part].mPropertyIDValid && mFrames[part].mPropertyID != 0;
    }
    //------------------------------------------------------------------------
    PropertyID StyleSectionFrameUnit::getImageSrc(FrameComType part) const
    {
        N_assert(part < FCT_Count, "error logic");
        return mFrames[part].mPropertyID;
    }
    //------------------------------------------------------------------------
    void StyleSectionFrameUnit::renderImpl(const Widget * dest, const Rectf & destRect,
        const ColourArea * cv, const Rectf * clipper) const
    {
        Rectf bgRect(destRect);
        Rectf farea;
        PlaneSizef imgsize;
        Vector2f imgoffset;
        ColourArea imgc;
        NIIf lf, rf, tf, bf;
        bool calcColoursPerImage;

        // vars we use to track what to do with the side pieces.
        NIIf topOffset = 0, bottomOffset = 0, leftOffset = 0, rightOffset = 0;
        NIIf topWidth, bottomWidth, leftHeight, rightHeight;
        topWidth = bottomWidth = destRect.getWidth();
        leftHeight = rightHeight = destRect.getHeight();

        // calculate final overall colours to be used
        ColourArea fc;
        getColourImpl(dest, cv, fc);

        if(fc.isOneColour())
        {
            calcColoursPerImage = false;
            imgc = fc;
        }
        else
        {
            calcColoursPerImage = true;
        }

        // top-left image
        if(const PixelBuffer * const comimg = getImage(FCT_TL, dest))
        {
            // calculate final destination area
            imgsize = comimg->getSize();
            imgoffset = comimg->getOffset();
            farea.mLeft = destRect.mLeft;
            farea.mTop = destRect.mTop;
            farea.setSize(imgsize);
            farea = destRect.intersects(farea);

            // update adjustments required to edges do to presence of this element.
            topOffset += imgsize.mWidth + imgoffset.x;
            leftOffset += imgsize.mHeight + imgoffset.y;
            topWidth -= topOffset;
            leftHeight -= leftOffset;

            // calculate colours that are to be used to this component image
            if(calcColoursPerImage)
            {
                lf = (farea.left() + imgoffset.x) / destRect.getWidth();
                rf = lf + farea.getWidth() / destRect.getWidth();
                tf = (farea.top() + imgoffset.y) / destRect.getHeight();
                bf = tf + farea.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf, tf, rf, bf));
            }
            dest->getPixelData()->add(*comimg, farea, clipper, imgc);
        }

        // top-right image
        if(const PixelBuffer * comimg = getImage(FCT_TR, dest))
        {
            // calculate final destination area
            imgsize = comimg->getSize();
            imgoffset = comimg->getOffset();
            farea.setLeft(destRect.right() - imgsize.mWidth);
            farea.setTop(destRect.top());
            farea.setSize(imgsize);
            farea = destRect.intersects(farea);

            // update adjustments required to edges do to presence of this element.
            rightOffset += imgsize.mHeight + imgoffset.y;
            topWidth -= imgsize.mWidth - imgoffset.x;
            rightHeight -= rightOffset;

            // calculate colours that are to be used to this component image
            if(calcColoursPerImage)
            {
                lf = (farea.left() + imgoffset.x) / destRect.getWidth();
                rf = lf + farea.getWidth() / destRect.getWidth();
                tf = (farea.top() + imgoffset.y) / destRect.getHeight();
                bf = tf + farea.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf, tf, rf, bf));
            }
            dest->getPixelData()->add(*comimg, farea, clipper, imgc);
        }

        // bottom-left image
        if(const PixelBuffer * comimg = getImage(FCT_BL, dest))
        {
            // calculate final destination area
            imgsize = comimg->getSize();
            imgoffset = comimg->getOffset();
            farea.setLeft(destRect.left());
            farea.setTop(destRect.bottom() - imgsize.mHeight);
            farea.setSize(imgsize);
            farea = destRect.intersects(farea);

            // update adjustments required to edges do to presence of this element.
            bottomOffset += imgsize.mWidth + imgoffset.x;
            bottomWidth -= bottomOffset;
            leftHeight -= imgsize.mHeight - imgoffset.y;

            // calculate colours that are to be used to this component image
            if(calcColoursPerImage)
            {
                lf = (farea.left() + imgoffset.x) / destRect.getWidth();
                rf = lf + farea.getWidth() / destRect.getWidth();
                tf = (farea.top() + imgoffset.y) / destRect.getHeight();
                bf = tf + farea.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf, tf, rf, bf));
            }
            dest->getPixelData()->add(*comimg, farea, clipper, imgc);
        }

        // bottom-right image
        if(const PixelBuffer * comimg = getImage(FCT_BR, dest))
        {
            // calculate final destination area
            imgsize = comimg->getSize();
            imgoffset = comimg->getOffset();
            farea.setLeft(destRect.right() - imgsize.mWidth);
            farea.setTop(destRect.bottom() - imgsize.mHeight);
            farea.setSize(imgsize);
            farea = destRect.intersects(farea);

            // update adjustments required to edges do to presence of this element.
            bottomWidth -= imgsize.mWidth - imgoffset.x;
            rightHeight -= imgsize.mHeight - imgoffset.y;

            // calculate colours that are to be used to this component image
            if (calcColoursPerImage)
            {
                lf = (farea.left() + comimg->getOffset().x) / destRect.getWidth();
                rf = lf + farea.getWidth() / destRect.getWidth();
                tf = (farea.top() + comimg->getOffset().y) / destRect.getHeight();
                bf = tf + farea.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf, tf, rf, bf));
            }

            // draw this element.
            dest->getPixelData()->add(*comimg, farea, clipper, imgc);
        }

        // top image
        if(const PixelBuffer * const comimg = getImage(FCT_Top, dest))
        {
            // calculate final destination area
            imgsize = comimg->getSize();
            farea.setLeft(destRect.left() + topOffset);
            farea.setRight(farea.left() + topWidth);
            farea.setTop(destRect.top());
            farea.setBottom(farea.top() + imgsize.mHeight);
            farea = destRect.intersects(farea);

            // adjust background area to miss this edge
            bgRect.mTop += imgsize.mHeight + comimg->getOffset().y;

            // calculate colours that are to be used to this component image
            if(calcColoursPerImage)
            {
                lf = (farea.left() + comimg->getOffset().x) / destRect.getWidth();
                rf = lf + farea.getWidth() / destRect.getWidth();
                tf = (farea.top() + comimg->getOffset().y) / destRect.getHeight();
                bf = tf + farea.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf, tf, rf, bf));
            }

            // draw this element.
            renderImage(*dest->getPixelData(), comimg, VL_Top,
                mTopLayout.get(dest), farea, imgc, clipper);
        }

        // bottom image
        if(const PixelBuffer * const comimg = getImage(FCT_Bottom, dest))
        {
            // calculate final destination area
            imgsize = comimg->getSize();
            farea.setLeft(destRect.left() + bottomOffset);
            farea.setRight(farea.left() + bottomWidth);
            farea.setBottom(destRect.bottom());
            farea.setTop(farea.bottom() - imgsize.mHeight);
            farea = destRect.intersects (farea);

            // adjust background area to miss this edge
            bgRect.mBottom -= imgsize.mHeight - comimg->getOffset().y;;

            // calculate colours that are to be used to this component image
            if(calcColoursPerImage)
            {
                lf = (farea.left() + comimg->getOffset().x) / destRect.getWidth();
                rf = lf + farea.getWidth() / destRect.getWidth();
                tf = (farea.top() + comimg->getOffset().y) / destRect.getHeight();
                bf = tf + farea.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf, tf, rf, bf));
            }

            // draw this element.
            renderImage(*dest->getPixelData(), comimg, VL_Bottom,
                mBottomLayout.get(dest), farea, imgc, clipper);
        }

        // left image
        if(const PixelBuffer * const comimg = getImage(FCT_Left, dest))
        {
            // calculate final destination area
            imgsize = comimg->getSize();
            farea.setLeft(destRect.left());
            farea.setRight(farea.left() + imgsize.mWidth);
            farea.setTop(destRect.top() + leftOffset);
            farea.setBottom(farea.top() + leftHeight);
            farea = destRect.intersects(farea);

            // adjust background area to miss this edge
            bgRect.mLeft += imgsize.mWidth + comimg->getOffset().x;

            // calculate colours that are to be used to this component image
            if(calcColoursPerImage)
            {
                lf = (farea.left() + comimg->getOffset().x) / destRect.getWidth();
                rf = lf + farea.getWidth() / destRect.getWidth();
                tf = (farea.top() + comimg->getOffset().y) / destRect.getHeight();
                bf = tf + farea.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf,  tf, rf, bf));
            }

            // draw this element.
            renderImage(*dest->getPixelData(), comimg, mLeftLayout.get(dest),
                HL_Left, farea, imgc, clipper);
        }

        // right image
        if(const PixelBuffer * const comimg = getImage(FCT_Right, dest))
        {
            // calculate final destination area
            imgsize = comimg->getSize();
            farea.setTop(destRect.top() + rightOffset);
            farea.setBottom(farea.top() + rightHeight);
            farea.setRight(destRect.right());
            farea.setLeft(farea.right() - imgsize.mWidth);
            farea = destRect.intersects (farea);

            // adjust background area to miss this edge
            bgRect.mRight -= imgsize.mWidth - comimg->getOffset().x;

            // calculate colours that are to be used to this component image
            if(calcColoursPerImage)
            {
                lf = (farea.left() + comimg->getOffset().x) / destRect.getWidth();
                rf = lf + farea.getWidth() / destRect.getWidth();
                tf = (farea.top() + comimg->getOffset().y) / destRect.getHeight();
                bf = tf + farea.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf, tf, rf, bf));
            }

            // draw this element.
            renderImage(*dest->getPixelData(), comimg, mRightLayout.get(dest),
                HL_Right, farea, imgc, clipper);
        }

        if(const PixelBuffer * const comimg = getImage(FCT_Bg, dest))
        {
            // calculate colours that are to be used to this component image
            if(calcColoursPerImage)
            {
                lf = (bgRect.left() + comimg->getOffset().x) / destRect.getWidth();
                rf = lf + bgRect.getWidth() / destRect.getWidth();
                tf = (bgRect.top() + comimg->getOffset().y) / destRect.getHeight();
                bf = tf + bgRect.getHeight() / destRect.getHeight();

                imgc = fc.get(Rectf(lf,  tf,rf, bf));
            }

            const HLayout horzFormatting = mBgHLayout.get(dest);

            const VLayout vertFormatting = mBgVLayout.get(dest);

            renderImage(*dest->getPixelData(), comimg, vertFormatting,
                horzFormatting, bgRect, imgc, clipper);
        }
    }
    //------------------------------------------------------------------------
    void StyleSectionFrameUnit::renderImage(GeometryPixel & buffer, const PixelBuffer * pb,
        VLayout vertFmt, HLayout horzFmt, Rectf & destRect, const ColourArea & cv,
            const Rectf * clipper) const
    {
        Nui32 horzTiles, vertTiles;
        NIIf xpos, ypos;

        PlaneSizef imgSz(pb->getSize());

        // calculate initial x co-ordinate and horizontal tile count according to formatting options
        switch(horzFmt)
        {
        case HL_Expand:
            imgSz.mWidth = destRect.getWidth();
            xpos = destRect.left();
            horzTiles = 1;
            break;
        case HL_Tile:
            xpos = destRect.left();
            horzTiles = std::abs(static_cast<int>(
                (destRect.getWidth() + (imgSz.mWidth - 1)) / imgSz.mWidth));
            break;
        case HL_Left:
            xpos = destRect.left();
            horzTiles = 1;
            break;
        case HL_Centre:
            xpos = destRect.left() + N_ToPixel((destRect.getWidth() - imgSz.mWidth) * 0.5f);
            horzTiles = 1;
            break;
        case HL_Right:
            xpos = destRect.right() - imgSz.mWidth;
            horzTiles = 1;
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("An unknown HLayout value was specified."));
        }

        // calculate initial y co-ordinate and vertical tile count according to formatting options
        switch(vertFmt)
        {
        case VL_Expand:
            imgSz.mHeight = destRect.getHeight();
            ypos = destRect.top();
            vertTiles = 1;
            break;
        case VL_Tile:
            ypos = destRect.top();
            vertTiles = std::abs(static_cast<int>(
                (destRect.getHeight() + (imgSz.mHeight - 1)) / imgSz.mHeight));
            break;
        case VL_Top:
            ypos = destRect.top();
            vertTiles = 1;
            break;
        case VL_Centre:
            ypos = destRect.top() + N_ToPixel((destRect.getHeight() - imgSz.mHeight) * 0.5f);
            vertTiles = 1;
            break;
        case VL_Bottom:
            ypos = destRect.bottom() - imgSz.mHeight;
            vertTiles = 1;
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("An unknown VLayout value was specified."));
        }

        // perform final rendering (actually is now a caching of the images which will be drawn)
        Rectf farea;
        Rectf finalClipper;
        const Rectf * clippingRect;
        farea.mTop = ypos;
        farea.mBottom = ypos + imgSz.mHeight;

        for(NCount row = 0; row < vertTiles; ++row)
        {
            farea.mLeft = xpos;
            farea.mRight = xpos + imgSz.mWidth;

            for (NCount col = 0; col < horzTiles; ++col)
            {
                // use custom clipping for right and bottom edges when tiling the imagery
                if(((vertFmt == VL_Tile) && row == vertTiles - 1) ||
                    ((horzFmt == HL_Tile) && col == horzTiles - 1))
                {
                    finalClipper = clipper ? clipper->intersects(destRect) : destRect;
                    clippingRect = &finalClipper;
                }
                // not tiliing, or not on far edges, just used passed in clipper (if any).
                else
                    clippingRect = clipper;

                buffer.add(*pb, farea, clippingRect, cv);

                farea.mLeft += imgSz.mWidth;
                farea.mRight += imgSz.mWidth;
            }

            farea.mTop += imgSz.mHeight;
            farea.mBottom += imgSz.mHeight;
        }
    }
    //------------------------------------------------------------------------
    bool StyleSectionFrameUnit::equal(const StyleSectionUnit & other) const
    {
        const StyleSectionFrameUnit * dst = dynamic_cast<const StyleSectionFrameUnit *>(&other);

        if(dst)
        {
            if (mBgVLayout != dst->mBgVLayout || mBgHLayout != dst->mBgHLayout)
                return false;

            if (mFrames[FCT_TL] != dst->mFrames[FCT_TL])
                return false;
            else if (mFrames[FCT_TR] != dst->mFrames[FCT_TR])
                return false;
            else if (mFrames[FCT_BL] != dst->mFrames[FCT_BL])
                return false;
            else if (mFrames[FCT_BR] != dst->mFrames[FCT_BR])
                return false;
            else if (mFrames[FCT_Left] != dst->mFrames[FCT_Left])
                return false;
            else if (mFrames[FCT_Right] != dst->mFrames[FCT_Right])
                return false;
            else if (mFrames[FCT_Top] != dst->mFrames[FCT_Top])
                return false;
            else if (mFrames[FCT_Bottom] != dst->mFrames[FCT_Bottom])
                return false;
            else if (mFrames[FCT_Bg] != dst->mFrames[FCT_Bg])
                return false;
            else
                return true;
        }
        return false;
    }
    //------------------------------------------------------------------------
    void StyleSectionFrameUnit::write(XmlSerializer * out) const
    {
        out->begin(_T("StyleSectionFrameUnit"));

        mArea.write(out);

        for(NCount i = 0; i < FCT_Count; ++i)
        {
            if (mFrames[i].mPropertyIDValid)
            {
                if (mFrames[i].mPropertyID == 0)
                    out->begin(_T("PixelBuffer"))
                        .attribute(_T("name"), mFrames[i].mImage->getID())
                        .attribute(_T("component"), NUI_conv(static_cast<FrameComType>(i)))
                        .end();
                else
                    out->begin(_T("PixelProperty"))
                        .attribute(_T("name"), mFrames[i].mPropertyID)
                        .attribute(_T("component"), NUI_conv(static_cast<FrameComType>(i)))
                        .end();
            }
        }

        writeColour(out);

        mBgVLayout.writeTag(out);
        mBgVLayout.writeValue(out);
        out->attribute(_T("component"), _T("Background")).end();

        mLeftLayout.writeTag(out);
        mLeftLayout.writeValue(out);
        out->attribute(_T("component"), _T("LeftEdge")).end();

        mRightLayout.writeTag(out);
        mRightLayout.writeValue(out);
        out->attribute(_T("component"), _T("RightEdge")).end();

        mBgHLayout.writeTag(out);
        mBgHLayout.writeValue(out);
        out->attribute(_T("component"), _T("Background")).end();

        mTopLayout.writeTag(out);
        mTopLayout.writeValue(out);
        out->attribute(_T("component"), _T("TopEdge")).end();

        mBottomLayout.writeTag(out);
        mBottomLayout.writeValue(out);
        out->attribute(_T("component"), _T("BottomEdge")).end();

        out->end();
    }
    //------------------------------------------------------------------------
}
}