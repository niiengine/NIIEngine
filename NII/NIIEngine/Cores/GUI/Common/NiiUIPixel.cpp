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
#include "NiiUIPixel.h"
#include "NiiUIManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiFontManager.h"
#include "NiiUIException.h"
#include "NiiPixelManager.h"
#include "NiiUISheet.h"
#include "NiiUIWidget.h"
#include "NiiString.h"
#include "NiiGeometryPixel.h"
#include "NiiUIContainer.h"
#include "NiiPixelBuffer.h"
#include "NiiLogManager.h"
#include "NiiUIStrConv.h"
#include "NiiScriptHelperManager.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PixelUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PixelUnit::PixelUnit() :
        mPadding(0, 0, 0, 0),
        mVLayout(VL_Bottom),
        mFixedAspect(false),
        mSelectPixel(0)
    {
    }
    //------------------------------------------------------------------------
    PixelUnit::~PixelUnit()
    {
    }
    //------------------------------------------------------------------------
    PixelUnit * PixelUnit::split(const Widget * , NIIf, Nidx)
    {
        N_EXCEPT(InvalidRequest, _I18n("this component does not support being split."));
    }
    //------------------------------------------------------------------------
    NCount PixelUnit::getBlankCount() const
    {
        return 0;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    NCount tpu_getNextTokenLength(const String & text, NCount begin)
    {
        String::size_type word_start = text.find_first_not_of(StrUtil::Delimiter, begin);

        if(word_start == String::npos)
            word_start = begin;

        String::size_type word_end = text.find_first_of(StrUtil::Delimiter, word_start);

        if(word_end == String::npos)
            word_end = text.length();

        return word_end - begin;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TextPixelUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TextPixelUnit::TextPixelUnit() :
        mFont(0),
        mColour(WidgetManager::DefaultTextColour),
        mSelectBegin(0),
        mSelectLength(0){}
    //------------------------------------------------------------------------
    TextPixelUnit::TextPixelUnit(const String & text) :
        mText(text),
        mFont(0),
        mColour(WidgetManager::DefaultTextColour),
        mSelectBegin(0),
        mSelectLength(0){}
    //------------------------------------------------------------------------
    TextPixelUnit::TextPixelUnit(const String & text, const Font * font) :
        mText(text),
        mFont(font),
        mColour(WidgetManager::DefaultTextColour),
        mSelectBegin(0),
        mSelectLength(0)
    {
    }
    //------------------------------------------------------------------------
    void TextPixelUnit::setSelect(const Widget * widget, NIIf start, NIIf end)
    {
        if(start >= end)
        {
            mSelectBegin = mSelectLength = 0;
            return;
        }

        const Font * fnt = getDestFont(widget);

        mSelectBegin = fnt->getCharAtPixel(mText, start);
        mSelectLength = fnt->getCharAtPixel(mText, end) - mSelectBegin + 1;
    }
    //------------------------------------------------------------------------
    const Font * TextPixelUnit::getDestFont(const Widget * widget) const
    {
        const Font * re;
        if(mFont)
            re = mFont;
        if(widget)
            re = widget->getSheet()->getFont();
        else
            re = N_Only(UI).getMainSheet()->getFont();
        return re;
    }
    //------------------------------------------------------------------------
    void TextPixelUnit::render(GeometryPixel * buffer, const Widget * ref_wnd,
        const Rectf * clip, const Vector2f & position, NIIf vertical_space,
            NIIf space_extra, const ColourArea * mc) const
    {
        const Font * fnt = getDestFont(ref_wnd);

        if(!fnt)
            return;

        Vector2f final_pos(position);
        NIIf y_scale = 1.0f;

        // handle formatting options
        switch(mVLayout)
        {
        case VL_Bottom:
            final_pos.y += vertical_space - getPixelSize(ref_wnd).mHeight;
            break;
        case VL_Centre:
            final_pos.y += (vertical_space - getPixelSize(ref_wnd).mHeight) / 2 ;
            break;
        case VL_Expand:
            y_scale = vertical_space / getPixelSize(ref_wnd).mHeight;
            break;
        case VL_Top:
            // nothing additional to do for this formatting option.
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("unknown VLayout option specified."));
        }

        // apply padding to position:
        final_pos += mPadding.getPos();

        // apply modulative colours if needed.
        ColourArea final_cols(mColour);
        if(mc)
            final_cols *= *mc;

        // render selection
        if(mSelectPixel && mSelectLength)
        {
            NIIf sel_start_extent = 0, sel_end_extent = 0;

            if (mSelectBegin > 0)
                sel_start_extent = fnt->getTextExtent(mText.substr(0, mSelectBegin));

            sel_end_extent = fnt->getTextExtent(mText.substr(0, mSelectBegin + mSelectLength));

            Rectf sel_rect(position.x + sel_start_extent, position.y,
                position.x + sel_end_extent, position.y + vertical_space);

            buffer->add(*mSelectPixel, sel_rect, clip, ColourArea(0xFF002FFF));
        }

        // draw the text string.
        fnt->drawText(buffer, mText, final_pos, clip, final_cols, space_extra, 1.0f, y_scale);
    }
    //------------------------------------------------------------------------
    PlaneSizef TextPixelUnit::getPixelSize(const Widget * target) const
    {
        const Font * fnt = getDestFont(target);

        PlaneSizef psz(mPadding.mLeft + mPadding.mRight, mPadding.mTop + mPadding.mBottom);

        if(fnt)
        {
            psz.mWidth += fnt->getTextExtent(mText);
            psz.mHeight += fnt->getFontHeight();
        }

        return psz;
    }
    //------------------------------------------------------------------------
    bool TextPixelUnit::isMultiUnit() const
    {
        return mText.length() > 1;
    }
    //------------------------------------------------------------------------
    PixelUnit * TextPixelUnit::split(const Widget * widget, NIIf split_point, Nidx first_component)
    {
        const Font * fnt = getDestFont(widget);
        if(!fnt)
            N_EXCEPT(InvalidRequest, _I18n("unable to split with no font set."));

        TextPixelUnit * leftunit = N_new TextPixelUnit();
        leftunit->mPadding = mPadding;
        leftunit->mVLayout = mVLayout;
        leftunit->mFont = mFont;
        leftunit->mColour = mColour;

        NCount left_len = 0;
        NIIf left_extent = 0.0f;

        while (left_len < mText.length())
        {
            NCount token_len = tpu_getNextTokenLength(mText, left_len);
            if (token_len == 0)
                break;

            const NIIf token_extent = fnt->getTextExtent(mText.substr(left_len, token_len));

            // does the next token extend past the split point?
            if(left_extent + token_extent > split_point)
            {
                // if it was the first token, split the token itself
                if(first_component == 0 && left_len == 0)
                    left_len = N_MAX(NCount, 1, fnt->getCharAtPixel(mText.substr(0, token_len), split_point));

                // left_len is now the character index at which to split the line
                break;
            }

            // add this token to the left side
            left_len += token_len;
            left_extent += token_extent;
        }

        // perform the split.
        leftunit->mText = mText.substr(0, left_len);

        // here we're trimming leading delimiters from the substring range
        NCount rhs_start = mText.find_first_not_of(StrUtil::Delimiter, left_len);
        if(rhs_start == String::npos)
            rhs_start = left_len;

        if(mSelectLength)
        {
            const NCount sel_end = mSelectBegin + mSelectLength - 1;
            leftunit->mSelectBegin = mSelectBegin;
            leftunit->mSelectLength = sel_end < left_len ? mSelectLength : left_len - mSelectBegin;

            if(sel_end >= left_len)
            {
                mSelectBegin = 0;
                mSelectLength -= rhs_start;
            }
            else
                setSelect(widget, 0, 0);
        }

        mText = mText.substr(rhs_start);

        return leftunit;
    }
    //------------------------------------------------------------------------
    PixelUnit * TextPixelUnit::clone() const
    {
        TextPixelUnit * c = N_new TextPixelUnit(*this);
        return c;
    }
    //------------------------------------------------------------------------
    NCount TextPixelUnit::getBlankCount() const
    {
        // TODO: The value calculated here is a good candidate for caching.

        NCount space_count = 0;

        // Count the number of spaces in this component.
        // NB: here I'm not countng tabs since those are really intended to be
        // something other than just a bigger space.
        NCount char_count = mText.length();
        for(NCount c = 0; c < char_count; ++c)
            if (mText[c] == ' ') // TODO: There are other space characters!
                ++space_count;

        return space_count;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // ImagePixelUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    ImagePixelUnit::ImagePixelUnit() :
        mImage(0),
        mColour(0xFFFFFFFF),
        mDestSize(0, 0),
        mSelect(false)
    {
    }
    //------------------------------------------------------------------------
    ImagePixelUnit::ImagePixelUnit(const PixelBuffer * pb) :
        mImage(pb),
        mColour(0xFFFFFFFF),
        mDestSize(0, 0),
        mSelect(false)
    {
    }
    //------------------------------------------------------------------------
    void ImagePixelUnit::setSelect(const Widget * , NIIf start, NIIf end)
    {
        mSelect = (start != end);
    }
    //------------------------------------------------------------------------
    void ImagePixelUnit::render(GeometryPixel * buffer, const Widget * ref_wnd,
        const Rectf * clip_rect, const Vector2f & position, NIIf vertical_space,
             NIIf, const ColourArea * mc) const
    {
        if(!mImage)
            return;

        Rectf dest(position.x, position.y, 0, 0);
        NIIf y_scale = 1.0f;

        // handle formatting options
        switch(mVLayout)
        {
        case VL_Bottom:
            dest.mTop += vertical_space - getPixelSize(ref_wnd).mHeight;
            break;
        case VL_Centre:
            dest.mTop += (vertical_space - getPixelSize(ref_wnd).mHeight) / 2 ;
            break;
        case VL_Expand:
            y_scale = vertical_space / getPixelSize(ref_wnd).mHeight;
            break;
        case VL_Top:
            // nothing additional to do for this formatting option.
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("unknown VLayout option specified."));
        }

        PlaneSizef sz(mImage->getSize());
        if(mDestSize.mWidth != 0.0)
            sz.mWidth = mDestSize.mWidth;
        if(mDestSize.mHeight != 0.0)
            sz.mHeight = mDestSize.mHeight;

        sz.mHeight *= y_scale;
        dest.setSize(sz);

        // apply padding to position
        dest += mPadding.getPos();

        // render the selection if needed
        if(mSelectPixel && mSelect)
        {
            const Rectf select_area(position, getPixelSize(ref_wnd));
            buffer->add(*mSelectPixel, select_area, clip_rect, ColourArea(0xFF002FFF));
        }

        // apply modulative colours if needed.
        ColourArea final_cols(mColour);
        if(mc)
            final_cols *= *mc;

        // draw the pb.
        buffer->add(*mImage, dest, clip_rect, final_cols);
    }
    //------------------------------------------------------------------------
    PlaneSizef ImagePixelUnit::getPixelSize(const Widget *) const
    {
        PlaneSizef sz(0, 0);

        if(mImage)
        {
            sz = mImage->getSize();
            if(mDestSize.mWidth != 0.0)
                sz.mWidth = mDestSize.mWidth;
            if(mDestSize.mHeight != 0.0)
                sz.mHeight = mDestSize.mHeight;
            sz.mWidth += (mPadding.mLeft + mPadding.mRight);
            sz.mHeight += (mPadding.mTop + mPadding.mBottom);
        }

        return sz;
    }
    //------------------------------------------------------------------------
    bool ImagePixelUnit::isMultiUnit() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    PixelUnit * ImagePixelUnit::clone() const
    {
        return N_new ImagePixelUnit(*this);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // WidgetPixelUnit
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    WidgetPixelUnit::WidgetPixelUnit() :
        mTargetValid(true),
        mTarget(0),
        mSelect(false){}
    //------------------------------------------------------------------------
    WidgetPixelUnit::WidgetPixelUnit(WidgetID wid) :
        mWidgetID(wid),
        mTarget(0),
        mTargetValid(false),
        mSelect(false){}
    //------------------------------------------------------------------------
    WidgetPixelUnit::WidgetPixelUnit(Widget * widget) :
        mTarget(widget),
        mTargetValid(true),
        mSelect(false){}
    //------------------------------------------------------------------------
    void WidgetPixelUnit::setWindow(WidgetID wid)
    {
        mWidgetID = wid;
        mTargetValid = false;
        mTarget = 0;
    }
    //------------------------------------------------------------------------
    void WidgetPixelUnit::setWindow(Widget * widget)
    {
        mTarget = widget;
        mTargetValid = true;
    }
    //------------------------------------------------------------------------
    const Widget * WidgetPixelUnit::getWindow() const
    {
        return getTarget(0);
    }
    //------------------------------------------------------------------------
    void WidgetPixelUnit::setSelect(const Widget *, NIIf start, NIIf end)
    {
        mSelect = (start != end);
    }
    //------------------------------------------------------------------------
    void WidgetPixelUnit::render(GeometryPixel * buffer, const Widget * ref_wnd, 
        const Rectf * clip_rect, const Vector2f & position, NIIf vertical_space, 
            NIIf, const ColourArea *) const
    {
        Widget * window = getTarget(ref_wnd);

        if(!window)
            return;

        // HACK: re-adjust for inner-rect of parent
        NIIf x_adj = 0, y_adj = 0;

        if(window->getParent())
        {
            const Rectf & outer(window->getParent()->getArea().getIntCache());
            const Rectf & inner(window->getParent()->getInnerArea().getIntCache());
            x_adj = inner.mLeft - outer.mLeft;
            y_adj = inner.mTop - outer.mTop;
        }
        // HACK: re-adjust for inner-rect of parent (Ends)

        Vector2f final_pos(position);
        // handle formatting options
        switch(mVLayout)
        {
        case VL_Bottom:
            final_pos.y += vertical_space - getPixelSize(ref_wnd).mHeight;
            break;
        case VL_Expand:
            N_Only(Log).log(_I18n("VL_Expand specified but is unsupported for Widget types; ")
                _I18n("defaulting to VL_Centre instead."));

            // intentional fall-through.
        case VL_Centre:
            final_pos.y += (vertical_space - getPixelSize(ref_wnd).mHeight) / 2 ;
            break;
        case VL_Top:
            // nothing additional to do for this formatting option.
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("unknown VLayout option specified."));
        }

        // render the selection if needed
        if(mSelectPixel && mSelect)
        {
            const Rectf select_area(position, getPixelSize(ref_wnd));
            buffer->add(*mSelectPixel, select_area, clip_rect, ColourArea(0xFF002FFF));
        }

        // we do not actually draw the widget, we just move it into position.
        const RelVector2f wpos(RelPosf(0, final_pos.x + mPadding.mLeft - x_adj),
            RelPosf(0, final_pos.y + mPadding.mTop - y_adj));

        window->setPos(wpos);
    }
    //------------------------------------------------------------------------
    Widget * WidgetPixelUnit::getTarget(const Widget * dest) const
    {
        if(!mTargetValid)
        {
            if(!dest && dest->isContainer())
                return 0;

            mTarget = static_cast<const Container *>(dest)->get(mWidgetID);
            mTargetValid = true;
        }

        return mTarget;
    }
    //------------------------------------------------------------------------
    PlaneSizef WidgetPixelUnit::getPixelSize(const Widget * dest) const
    {
        PlaneSizef sz(0, 0);

        if(const Widget * window = getTarget(dest))
        {
            sz = window->getPixelSize();
            sz.mWidth += (mPadding.mLeft + mPadding.mRight);
            sz.mHeight += (mPadding.mTop + mPadding.mBottom);
        }

        return sz;
    }
    //------------------------------------------------------------------------
    bool WidgetPixelUnit::isMultiUnit() const
    {
        return false;
    }
    //------------------------------------------------------------------------
    PixelUnit * WidgetPixelUnit::clone() const
    {
        return N_new WidgetPixelUnit(*this);
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PixelUnitGrid
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PixelUnitGrid::PixelUnitGrid()
    {
        addEnter();
    }
    //------------------------------------------------------------------------
    PixelUnitGrid::~PixelUnitGrid()
    {
        removeAll();
    }
    //------------------------------------------------------------------------
    PixelUnitGrid::PixelUnitGrid(const PixelUnitGrid & o)
    {
        NCount i, iend = mUnitList.size();
        for(i = 0; i < iend; ++i)
            N_delete mUnitList[i];

        mUnitList.clear();
        iend = o.mUnitList.size();
        for(i = 0; i < iend; ++i)
        {
            mUnitList.push_back(o.mUnitList[i]->clone());
        }
        mLineList = o.mLineList;
    }
    //------------------------------------------------------------------------
    PixelUnitGrid & PixelUnitGrid::operator=(const PixelUnitGrid & o)
    {
        NCount i, iend = mUnitList.size();
        for(i = 0; i < iend; ++i)
            N_delete mUnitList[i];

        mUnitList.clear();
        iend = o.mUnitList.size();
        for(i = 0; i < iend; ++i)
        {
            mUnitList.push_back(o.mUnitList[i]->clone());
        }
        mLineList = o.mLineList;
        return *this;
    }
    //------------------------------------------------------------------------
    void PixelUnitGrid::add(const PixelUnit & com)
    {
        mUnitList.push_back(com.clone());
        ++mLineList.back().second;
    }
    //------------------------------------------------------------------------
    void PixelUnitGrid::removeAll()
    {
        NCount i, iend = mUnitList.size();
        for(i = 0; i < iend; ++i)
            N_delete mUnitList[i];

        mUnitList.clear();
    }
    //------------------------------------------------------------------------
    void PixelUnitGrid::split(const Widget * widget, NCount line, NIIf split_point, PixelUnitGrid & left)
    {
        // FIXME: This function is big and nasty; it breaks all the rules for a
        // 'good' function and desperately needs some refactoring work done to it.
        // On the plus side, it does seem to work though ;)

        if(line >= getLineCount())
            N_EXCEPT(InvalidRequest, _I18n("line number specified is invalid."));

        left.removeAll();

        if(mUnitList.empty())
            return;

        // move all components in lines prior to the line being split to the left
        if(line > 0)
        {
            // calculate size of range
            const NCount sz = mLineList[line - 1].first + mLineList[line - 1].second;
            UnitList::iterator cb = mUnitList.begin();
            UnitList::iterator ce = cb + sz;
            // copy components to left side
            left.mUnitList.assign(cb, ce);
            // erase components from this side.
            mUnitList.erase(cb, ce);

            LineList::iterator lb = mLineList.begin();
            LineList::iterator le = lb + line;
            // copy lines to left side
            left.mLineList.assign(lb, le);
            // erase lines from this side
            mLineList.erase(lb, le);
        }

        // find the component where the requested split point lies.
        NIIf partial_extent = 0;

        NCount idx = 0;
        const NCount last_component = mLineList[0].second;
        for(; idx < last_component; ++idx)
        {
            partial_extent += mUnitList[idx]->getPixelSize(widget).mWidth;

            if(split_point <= partial_extent)
                break;
        }

        // case where split point is past the end
        if(idx >= last_component)
        {
            // transfer this line's components to the 'left' string.
            //
            // calculate size of range
            const NCount sz = mLineList[0].second;
            // range start
            UnitList::iterator cb = mUnitList.begin();
            // range end (exclusive)
            UnitList::iterator ce = cb + sz;
            // copy components to left side
            left.mUnitList.insert(left.mUnitList.end(), cb, ce);
            // erase components from this side.
            mUnitList.erase(cb, ce);

            // copy line info to left side
            left.mLineList.push_back(mLineList[0]);
            // erase line from this side
            mLineList.erase(mLineList.begin());

            // fix up lines in this object
            for(NCount comp = 0, i = 0; i < mLineList.size(); ++i)
            {
                mLineList[i].first = comp;
                comp += mLineList[i].second;
            }

            return;
        }

        left.addEnter();
        const NCount left_line = left.getLineCount() - 1;
        // Everything up to 'idx' is xfered to 'left'
        for(NCount i = 0; i < idx; ++i)
        {
            left.mUnitList.push_back(mUnitList[0]);
            mUnitList.erase(mUnitList.begin());
            ++left.mLineList[left_line].second;
            --mLineList[0].second;
        }

        // now to split item 'idx' putting half in left and leaving half in this.
        PixelUnit * c = mUnitList[0];
        if(c->isMultiUnit())
        {
            PixelUnit * lc = c->split(widget, split_point - (partial_extent - c->getPixelSize(widget).mWidth), idx);

            if(lc)
            {
                left.mUnitList.push_back(lc);
                ++left.mLineList[left_line].second;
            }
        }
        // can't split, if component width is >= split_point xfer the whole
        // component to it's own line in the left part (FIX #306)
        else if(c->getPixelSize(widget).mWidth >= split_point)
        {
            left.addEnter();
            left.mUnitList.push_back(mUnitList[0]);
            mUnitList.erase(mUnitList.begin());
            ++left.mLineList[left_line + 1].second;
            --mLineList[0].second;
        }

        // fix up lines in this object
        for(NCount comp = 0, i = 0; i < mLineList.size(); ++i)
        {
            mLineList[i].first = comp;
            comp += mLineList[i].second;
        }
    }
    //------------------------------------------------------------------------
    void PixelUnitGrid::addEnter()
    {
        NCount first_component = mLineList.empty() ? 0 : mLineList.back().first + mLineList.back().second;

        mLineList.push_back(LineInfo(first_component, 0));
    }
    //------------------------------------------------------------------------
    PlaneSizef PixelUnitGrid::getPixelSize(const Widget * widget, NCount line) const
    {
        if (line >= getLineCount())
            N_EXCEPT(InvalidRequest, _I18n("line number specified is invalid."));

        PlaneSizef re(0, 0);

        NCount i, iend = mLineList[line].first + mLineList[line].second;
        for(i = mLineList[line].first; i < iend; ++i)
        {
            const PlaneSizef comp_sz(mUnitList[i]->getPixelSize(widget));
			re.mWidth += comp_sz.mWidth;

            if(comp_sz.mHeight > re.mHeight)
				re.mHeight = comp_sz.mHeight;
        }

        return re;
    }
    //------------------------------------------------------------------------
    NCount PixelUnitGrid::getSpaceCount(NCount line) const
    {
        if(line >= getLineCount())
            N_EXCEPT(InvalidRequest, _I18n("line number specified is invalid."));

        NCount space_count = 0;
        NCount i, iend = mLineList[line].first + mLineList[line].second;

        for(i = mLineList[line].first; i < iend; ++i)
            space_count += mUnitList[i]->getBlankCount();

        return space_count;
    }
    //------------------------------------------------------------------------
    void PixelUnitGrid::draw(const Widget * widget, NCount line, GeometryPixel & buffer,
        const Vector2f & position, const ColourArea * mc, const Rectf * clip_rect,  NIIf space_extra) const
    {
        if(line >= getLineCount())
            N_EXCEPT(InvalidRequest, _I18n("line number specified is invalid."));

        NIIf render_height = getPixelSize(widget, line).mHeight;

        Vector2f comp_pos(position);

        NCount i, iend = mLineList[line].first + mLineList[line].second;
        for(i = mLineList[line].first; i < iend; ++i)
        {
            mUnitList[i]->render(&buffer, widget, clip_rect, comp_pos, 
                render_height, space_extra, mc);
            comp_pos.x += mUnitList[i]->getPixelSize(widget).mWidth;
        }
    }
    //------------------------------------------------------------------------
    void PixelUnitGrid::setSelection(const Widget * widget, NIIf start, NIIf end)
    {
        const NCount last_component = mLineList[0].second;
        NIIf partial_extent = 0;
        NCount idx = 0;
        for(; idx < last_component; ++idx)
            if(start <= partial_extent + mUnitList[idx]->getPixelSize(widget).mWidth)
                break;

        start -= partial_extent;
        end -= partial_extent;

        while(end > 0.0f)
        {
            const NIIf comp_extent = mUnitList[idx]->getPixelSize(widget).mWidth;
            mUnitList[idx]->setSelect(widget, start, (end >= comp_extent) ? comp_extent : end);
            start = 0;
            end -= comp_extent;
            ++idx;
        }
    }
    //------------------------------------------------------------------------
    NIIf PixelUnitGrid::getHExtent(const Widget * widget) const
    {
        NIIf w = 0.0f;
        NCount i, iend = mLineList.size();
        for(i = 0; i < iend; ++i)
        {
            NIIf this_width = getPixelSize(widget, i).mWidth;
            if(this_width > w)
                w = this_width;
        }

        return w;
    }
    //------------------------------------------------------------------------
    NIIf PixelUnitGrid::getVExtent(const Widget * widget) const
    {
        NIIf h = 0.0f;
        NCount i, iend = mLineList.size();
        for(i = 0; i < mLineList.size(); ++i)
            h += getPixelSize(widget, i).mHeight;

        return h;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TextView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    TextView::~TextView()
    {
    }
    //------------------------------------------------------------------------
    PixelUnitGrid TextView::parse(const String & input, const Font * font, const ColourArea * colour)
    {
        PixelUnitGrid rs;

        NCount epos, spos = 0;

        while((epos = input.find('\n', spos)) != String::npos)
        {

            TextPixelUnit rstc(input.substr(spos, epos - spos), font);

            if(colour)
                rstc.setColour(*colour);

            rs.add(rstc);
            
            rs.addEnter();

            // set new start position (skipping the previous \n we found)
            spos = epos + 1;
        }

        if(spos < input.length())
        {

            TextPixelUnit rstc(input.substr(spos), font);

            if(colour)
                rstc.setColour(*colour);

            rs.add(rstc);
        }

        return rs;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // EffectTextView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    bool parse_section(String::const_iterator & pos, const String::const_iterator & end, String::value_type delim, String & out)
    {
        const String::value_type escape('\\');
        out.resize(0);

        String::const_iterator start_iter(pos);

        for ( ; pos != end; ++pos)
        {
            if (*pos == delim)
            {
                out.append(start_iter, pos++);
                return true;
            }

            if (*pos == escape)
            {
                out.append(start_iter, pos++);

                if (pos == end)
                    return false;

                start_iter = pos;
            }
        }

        out.append(start_iter, pos);
        return false;
    }
    //------------------------------------------------------------------------
    /*const String EffectTextView::ColourTagName(_T("colour"));
    const String EffectTextView::FontTagName(_T("font"));
    const String EffectTextView::ImageTagName(_T("image"));
    const String EffectTextView::WindowTagName(_T("window"));
    const String EffectTextView::VertAlignmentTagName(_T("vert-alignment"));
    const String EffectTextView::PaddingTagName(_T("padding"));
    const String EffectTextView::TopPaddingTagName(_T("top-padding"));
    const String EffectTextView::BottomPaddingTagName(_T("bottom-padding"));
    const String EffectTextView::LeftPaddingTagName(_T("left-padding"));
    const String EffectTextView::RightPaddingTagName(_T("right-padding"));
    const String EffectTextView::AspectLockTagName(_T("aspect-lock"));
    const String EffectTextView::ImageSizeTagName(_T("image-size"));
    const String EffectTextView::ImageWidthTagName(_T("image-width"));
    const String EffectTextView::ImageHeightTagName(_T("image-height"));
    const String EffectTextView::TopAlignedValueName(_T("top"));
    const String EffectTextView::BottomAlignedValueName(_T("bottom"));
    const String EffectTextView::CentreAlignedValueName(_T("centre"));
    const String EffectTextView::StretchAlignedValueName(_T("stretch"));*/
    //------------------------------------------------------------------------
    EffectTextView::EffectTextView() :
        ScriptProperty(N_CmdObj_UIPixelGrid),
        mSize(0, 0),
        mAspect(false)
    {
        mPadding = Rectf(0, 0, 0, 0);
        mRenderFont = mFont = 0;
        mColour.set(0xFFFFFFFF);
        mRenderColour = mColour;
        mVLayout = VL_Bottom;
    }
    //------------------------------------------------------------------------
    EffectTextView::EffectTextView(FontID font, const ColourArea & colour) :
        ScriptProperty(N_CmdObj_UIPixelGrid),
        mSize(0, 0),
        mAspect(false)
    {
        mPadding = Rectf(0, 0, 0, 0);
        mRenderFont = mFont = font;
        mRenderColour = mColour = colour;
        mVLayout = VL_Bottom;
    }
    //------------------------------------------------------------------------
    PixelUnitGrid EffectTextView::parse(const String & input_string, const Font * font, const ColourArea * colour)
    {
        mPadding = Rectf(0, 0, 0, 0);
        mRenderFont = mFont;
        mRenderColour = mColour;
        mSize.mWidth = mSize.mHeight = 0.0f;
        mVLayout = VL_Bottom;
        mAspect = false;

        if(font)
            mRenderFont = font->getID();

        if(colour)
            mColour = *colour;

        PixelUnitGrid rs;
        String curr_section, tag_string;

        for(String::const_iterator input_iter(input_string.begin()); input_iter != input_string.end(); )
        {
            const bool found_tag = parse_section(input_iter, input_string.end(), '[', curr_section);
            NCount cpos = 0;
            // split the given string into lines based upon the newline character
            while (curr_section.length() > cpos)
            {
                // find next newline
                const NCount nlpos = curr_section.find('\n', cpos);
                // calculate length of this substring
                const NCount len = ((nlpos != String::npos) ? nlpos : curr_section.length()) - cpos;

                // construct new curr_section component and append it.
                TextPixelUnit rtc(curr_section.substr(cpos, len), N_Only(Font).getFont(mRenderFont));
                rtc.setPadding(mPadding);
                rtc.setColour(mColour);
                rtc.setVLayout(mVLayout);
                rtc.setFixedAspect(mAspect);
                rs.add(rtc);

                // break line if needed
                if (nlpos != String::npos)
                    rs.addEnter();

                // advance current position.  +1 to skip the \n char
                cpos += len + 1;
            }

            if(!found_tag)
                return rs;

            if(!parse_section(input_iter, input_string.end(), ']', tag_string))
            {
                N_Only(Log).log(_I18n("Ignoring unterminated tag : [") + tag_string);

                return rs;
            }
            // all our default strings are of the form <var> = <val>
            // so do a quick check for the = char and abort if it's not there.
            if(String::npos == tag_string.find('='))
            {
                N_Only(Log).log(_I18n("unable to make sense of control string '") + tag_string + _I18n("'.  Ignoring!"));

                return rs;
            }

            Ntchar var_buf[128];
            Ntchar val_buf[128];
            Nsscanf(tag_string.c_str(), _T(" %127[^ =] = '%127[^']"), var_buf, val_buf);

            const String var_str(var_buf);
            const String val_str(val_buf);
            
            switch(getProperty(var_str))
            {
                case N_PropertyUIPixelGrid_Colour:
                {
                    Colour temp;
                    StrConv::conv(val_str, temp);
                    mColour.set(temp);
                }
                break;
                case N_PropertyUIPixelGrid_Font:
                {
                    StrConv::conv(val_str, mRenderFont);
                }
                break;
                case N_PropertyUIPixelGrid_Image:
                {
                    Nui32 rid, pid;
                    StringList temp;
                    StrUtil::split(val_str, temp, _T("|"));
                    StrConv::conv(temp[0], rid);
                    StrConv::conv(temp[1], pid);

                    ImagePixelUnit ric(N_Only(Pixel).get(rid, pid));
                    ric.setPadding(mPadding);
                    ric.setColour(mColour);
                    ric.setVLayout(mVLayout);
                    ric.setSize(mSize);
                    ric.setFixedAspect(mAspect);
                    rs.add(ric);
                }
                break;
                case N_PropertyUIPixelGrid_Window:
                {
                    Nui32 tempid;
                    StrConv::conv(val_str, tempid);
                    WidgetPixelUnit rwc(tempid);
                    rwc.setPadding(mPadding);
                    rwc.setVLayout(mVLayout);
                    rwc.setFixedAspect(mAspect);
                    rs.add(rwc);
                }
                break;
                case N_PropertyUIPixelGrid_VertAlignment:
                {
                    switch (getProperty(val_str))
                    {
                        case N_PropertyUIPixelGrid_TopAlignedValue:
                        {
                            mVLayout = VL_Top;
                        }
                        break;
                        case N_PropertyUIPixelGrid_BottomAlignedValue:
                        {
                            mVLayout = VL_Bottom;
                        }
                        break;
                        case N_PropertyUIPixelGrid_CentreAlignedValue:
                        {
                            mVLayout = VL_Centre;
                        }
                        break;
                        case N_PropertyUIPixelGrid_StretchAlignedValue:
                        {
                            mVLayout = VL_Expand;
                        }
                        break;
                        default:
                            N_Only(Log).log(_I18n("unknown vertical alignment '") + val_str + _I18n("'.  Ignoring!"));
                            break;
                    }
                }
                break;
                case N_PropertyUIPixelGrid_Padding:
                {
                    StrConv::conv(val_str, mPadding);
                }
                break;
                case N_PropertyUIPixelGrid_TopPadding:
                {
                    StrConv::conv(val_str, mPadding.mTop);
                }
                break;
                case N_PropertyUIPixelGrid_BottomPadding:
                {
                    StrConv::conv(val_str, mPadding.mBottom);
                }
                break;
                case N_PropertyUIPixelGrid_LeftPadding:
                {
                    StrConv::conv(val_str, mPadding.mLeft);
                }
                break;
                case N_PropertyUIPixelGrid_RightPadding:
                {
                    StrConv::conv(val_str, mPadding.mRight);
                }
                break;
                case N_PropertyUIPixelGrid_AspectLock:
                {
                    StrConv::conv(val_str, mAspect);
                }
                break;
                case N_PropertyUIPixelGrid_ImageSize:
                {
                    UI::StrConv::conv(val_str, mSize);
                }
                break;
                case N_PropertyUIPixelGrid_ImageWidth:
                {
                    StrConv::conv(val_str, mSize.mWidth);
                }
                break;
                case N_PropertyUIPixelGrid_ImageHeight:
                {
                    StrConv::conv(val_str, mSize.mHeight);
                }
                break;
                default:
                    N_Only(Log).log(_I18n("unknown control variable '") + var_str + _I18n("'.  Ignoring!"));
                break;
            }
        }
        return rs;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // PixelLayout
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    PixelLayout::PixelLayout(const PixelUnitGrid * grid) :
        mPixelGrid(grid)
    {
    }
    //------------------------------------------------------------------------
    PixelLayout::~PixelLayout()
    {
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // LeftLayout
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    LeftLayout::LeftLayout(const PixelUnitGrid * grid) :
        PixelLayout(grid)
    {
    }
    //------------------------------------------------------------------------
    void LeftLayout::layout(const Widget *, const PlaneSizef &)
    {
    }
    //------------------------------------------------------------------------
    void LeftLayout::draw(const Widget * dest, GeometryPixel & buffer,
        const Vector2f & position, const ColourArea * mc, const Rectf * clip_rect) const
    {
        Vector2f dpos(position);
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
        {
            mPixelGrid->draw(dest, i, buffer, dpos, mc, clip_rect, 0.0f);
            dpos.y += mPixelGrid->getPixelSize(dest, i).mHeight;
        }
    }
    //------------------------------------------------------------------------
    NIIf LeftLayout::getHExtent(const Widget * dest) const
    {
        NIIf w = 0.0f;
        NIIf finalw;
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
        {
            finalw = mPixelGrid->getPixelSize(dest, i).mWidth;
            if (finalw > w)
                w = finalw;
        }

        return w;
    }
    //------------------------------------------------------------------------
    NIIf LeftLayout::getVExtent(const Widget * dest) const
    {
        NIIf finalh = 0.0f;
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
            finalh += mPixelGrid->getPixelSize(dest, i).mHeight;

        return finalh;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // RightLayout
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    RightLayout::RightLayout(const PixelUnitGrid * grid) :
        PixelLayout(grid)
    {
    }
    //------------------------------------------------------------------------
    void RightLayout::layout(const Widget * dest, const PlaneSizef & area_size)
    {
        mSpaceCache.clear();
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
            mSpaceCache.push_back(area_size.mWidth - mPixelGrid->getPixelSize(dest, i).mWidth);
    }
    //------------------------------------------------------------------------
    void RightLayout::draw(const Widget * dest, GeometryPixel & buffer,
        const Vector2f & pos, const ColourArea * mc, const Rectf * clip_rect) const
    {
        Vector2f draw_pos;
        draw_pos.y = pos.y;
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
        {
            draw_pos.x = pos.x + mSpaceCache[i];
            mPixelGrid->draw(dest, i, buffer, draw_pos, mc, clip_rect, 0.0f);
            draw_pos.y += mPixelGrid->getPixelSize(dest, i).mHeight;
        }
    }
    //------------------------------------------------------------------------
    NIIf RightLayout::getHExtent(const Widget * dest) const
    {
        NIIf finalw = 0.0f;
        NIIf w;
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
        {
            w = mPixelGrid->getPixelSize(dest, i).mWidth;
            if(w > finalw)
                finalw = w;
        }

        return finalw;
    }
    //------------------------------------------------------------------------
    NIIf RightLayout::getVExtent(const Widget * dest) const
    {
        NIIf finalh = 0.0f;
        NCount i, iend = mPixelGrid->getLineCount();
        for (i = 0; i < iend; ++i)
            finalh += mPixelGrid->getPixelSize(dest, i).mHeight;

        return finalh;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CenterLayout
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    CenterLayout::CenterLayout(const PixelUnitGrid * grid) :
        PixelLayout(grid)
    {
    }
    //------------------------------------------------------------------------
    void CenterLayout::layout(const Widget * dest, const PlaneSizef & area_size)
    {
        mSpaceCache.clear();
        NCount i, iend = mPixelGrid->getLineCount();
        for (i = 0; i < iend; ++i)
            mSpaceCache.push_back((area_size.mWidth - mPixelGrid->getPixelSize(dest, i).mWidth) / 2.0f);
    }
    //------------------------------------------------------------------------
    void CenterLayout::draw(const Widget * dest, GeometryPixel & buffer,
        const Vector2f & position, const ColourArea * mc, const Rectf * clip_rect) const
    {
        Vector2f draw_pos;
        draw_pos.y = position.y;
        NCount i, iend = mPixelGrid->getLineCount();
        for (i = 0; i < iend; ++i)
        {
            draw_pos.x = position.x + mSpaceCache[i];
            mPixelGrid->draw(dest, i, buffer, draw_pos, mc, clip_rect, 0.0f);
            draw_pos.y += mPixelGrid->getPixelSize(dest, i).mHeight;
        }
    }
    //------------------------------------------------------------------------
    NIIf CenterLayout::getHExtent(const Widget * dest) const
    {
        NIIf w = 0.0f;
        NIIf finalw;
        NCount i, iend = mPixelGrid->getLineCount();
        for (i = 0; i < iend; ++i)
        {
            finalw = mPixelGrid->getPixelSize(dest, i).mWidth;
            if (finalw > w)
                w = finalw;
        }
        return w;
    }
    //------------------------------------------------------------------------
    NIIf CenterLayout::getVExtent(const Widget * dest) const
    {
        NIIf finalh = 0.0f;
        NCount i, iend = mPixelGrid->getLineCount();
        for (i = 0; i < iend; ++i)
            finalh += mPixelGrid->getPixelSize(dest, i).mHeight;

        return finalh;
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // JustifyLayout
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    JustifyLayout::JustifyLayout(const PixelUnitGrid * grid) :
        PixelLayout(grid)
    {
    }
    //------------------------------------------------------------------------
    void JustifyLayout::layout(const Widget * dest, const PlaneSizef & area_size)
    {
        mSpaceCache.clear();
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
        {
            const NCount space_count = mPixelGrid->getSpaceCount(i);
            const NIIf string_width = mPixelGrid->getPixelSize(dest, i).mWidth;

            if((space_count == 0) || (string_width >= area_size.mWidth))
                mSpaceCache.push_back(0.0f);
            else
                mSpaceCache.push_back((area_size.mWidth - string_width) / space_count);
        }
    }
    //------------------------------------------------------------------------
    void JustifyLayout::draw(const Widget * dest, GeometryPixel & buffer,
        const Vector2f & position, const ColourArea * mc, const Rectf * clip_rect) const
    {
        Vector2f draw_pos(position);
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
        {
            mPixelGrid->draw(dest, i, buffer, draw_pos, mc, clip_rect, mSpaceCache[i]);
            draw_pos.y += mPixelGrid->getPixelSize(dest, i).mHeight;
        }
    }
    //------------------------------------------------------------------------
    NIIf JustifyLayout::getHExtent(const Widget * dest) const
    {
        NIIf w = 0.0f;
        NCount i, iend = mPixelGrid->getLineCount();
        for(i = 0; i < iend; ++i)
        {
            NIIf this_width = mPixelGrid->getPixelSize(dest, i).mWidth +
                mPixelGrid->getSpaceCount(i) * mSpaceCache[i];

            if(this_width > w)
                w = this_width;
        }

        return w;
    }
    //------------------------------------------------------------------------
    NIIf JustifyLayout::getVExtent(const Widget * dest) const
    {
        NIIf h = 0.0f;
        NCount i, iend = mPixelGrid->getLineCount();
        for (i = 0; i < iend; ++i)
            h += mPixelGrid->getPixelSize(dest, i).mHeight;

        return h;
    }
    //------------------------------------------------------------------------
    template<> void WrapLayout<JustifyLayout>::layout(const Widget * dest, const PlaneSizef & area_size)
    {
        NCount i, iend = mLineList.size();
        for (i = 0; i < iend; ++i)
        {
            const PixelUnitGrid * rs = mLineList[i]->getGrid();
            N_delete rs;
            N_delete mLineList[i];
        }
        mLineList.clear();

        PixelUnitGrid rstring, lstring;
        rstring = *mPixelGrid;
        NIIf rs_width;

        PixelLayout * frs;
        NCount l, lend = rstring.getLineCount();
        for (l = 0; l < lend; ++l)
        {
            while ((rs_width = rstring.getPixelSize(dest, l).mWidth) > 0)
            {
                // skip line if no wrapping occurs
                if (rs_width <= area_size.mWidth)
                    break;

                // split rstring at width into lstring and remaining rstring
                rstring.split(dest, l, area_size.mWidth, lstring);
                frs = N_new JustifyLayout(N_new PixelUnitGrid(lstring));
                frs->layout(dest, area_size);
                mLineList.push_back(frs);
                l = 0;
            }
        }

        // last line (which we do not justify)
        frs = N_new LeftLayout(N_new PixelUnitGrid(rstring));
        frs->layout(dest, area_size);
        mLineList.push_back(frs);
    }
    //------------------------------------------------------------------------
}
}