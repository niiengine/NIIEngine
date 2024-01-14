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
#include "NiiUIMultiLineEditbox.h"
#include "NiiUIScrollbar.h"
#include "NiiUIException.h"
#include "NiiUIManager.h"
#include "NiiUIWidgetList.h"
#include "NiiString.h"
#include "NiiPixelManager.h"
#include "NiiFont.h"
#include "NiiUIWidgetManager.h"
#include "NiiPixelBuffer.h"

#define TextArea                1
#define TextAreaScroll          2
#define TextAreaHScroll         3
#define TextAreaVScroll         4
#define TextAreaHVScroll        5

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdReadOnly : public PropertyCmd
    {
    public:
        CmdReadOnly() : PropertyCmd(N_PropertyUIMultiLineEditbox_ReadOnly, _T("read_only"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiEditbox *>(own)->isReadOnly(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiEditbox *>(own)->setReadOnly(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiEditbox *>(own)->isReadOnly();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiEditbox *>(own)->setReadOnly(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdReadOnly();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdCaretIndex : public PropertyCmd
    {
    public:
        CmdCaretIndex() : PropertyCmd(N_PropertyUIMultiLineEditbox_CaretIndex, _T("caret_index"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiEditbox *>(own)->getInput(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<MultiEditbox *>(own)->setInput(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const MultiEditbox *>(own)->getInput();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<MultiEditbox *>(own)->setInput(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCaretIndex();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((Nui32)0, out);
        }
    };

    class _EngineInner CmdSelectionStart : public PropertyCmd
    {
    public:
        CmdSelectionStart() : PropertyCmd(N_PropertyUIMultiLineEditbox_SelectionStartIndex, _T("selection_start"), _I18n(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiEditbox *>(own)->getSelectBegin(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<MultiEditbox *>(own)->setSelectBegin(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const MultiEditbox *>(own)->getSelectBegin();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<MultiEditbox *>(own)->setSelectBegin(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectionStart();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NCount)0, out);
        }
    };

    class _EngineInner CmdSelectionLength : public PropertyCmd
    {
    public:
        CmdSelectionLength() : PropertyCmd(N_PropertyUIMultiLineEditbox_SelectionLength, _T("selection_length"),
            _T("Property to get/set the length of the selection (as a count of the number of code points selected).  Value is \"[Nui32]\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiEditbox *>(own)->getSelectLength(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<MultiEditbox *>(own)->setSelectLength(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const MultiEditbox *>(own)->getSelectLength();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<MultiEditbox *>(own)->setSelectLength(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectionLength();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NCount)0, out);
        }
    };

    class _EngineInner CmdMaxTextLength : public PropertyCmd
    {
    public:
        CmdMaxTextLength() : PropertyCmd(N_PropertyUIMultiLineEditbox_MaxTextLength, _T("max_text_length"),
            _T("Property to get/set the the maximum allowed text length (as a count of code points).  Value is \"[Nui32]\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiEditbox *>(own)->getMaxLength(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<MultiEditbox *>(own)->setMaxLength(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const MultiEditbox *>(own)->getMaxLength();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<MultiEditbox *>(own)->setMaxLength(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxTextLength();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NCount)out.max_size(), out);
        }
    };

    class _EngineInner CmdWordWrap : public PropertyCmd
    {
    public:
        CmdWordWrap() : PropertyCmd(N_PropertyUIMultiLineEditbox_WordWrapped, _T("word_wrap"),
            _T("Property to get/set the word-wrap setting of the edit box.  Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiEditbox *>(own)->isWordWrap(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiEditbox *>(own)->setWordWrap(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiEditbox *>(own)->isWordWrap();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiEditbox *>(own)->setWordWrap(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdWordWrap();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };

    class _EngineInner CmdSelectionBrushImage : public PropertyCmd
    {
    public:
        CmdSelectionBrushImage() : PropertyCmd(N_PropertyUIMultiLineEditbox_SelectionImage, _T("selection_brush_image"),
            _T("Property to get/set the selection brush image for the editbox.  Value should be \"set:[imageset name] image:[image name]\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            const MultiEditbox * obj = static_cast<const MultiEditbox *>(own);
            PixelBuffer * val = obj->getSelectView();
            out = val ? N_conv(val->getGroupID()) + _T("|") + N_conv(val->getID()) : _T("0|0");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));

            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);
            static_cast<MultiEditbox *>(own)->setSelectView(N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            const MultiEditbox * obj = static_cast<const MultiEditbox *>(own);
            PixelBuffer * val = obj->getSelectView();
            Nui64 * temp = (Nui64 *)out;
            *temp = val ? ((Nui64)val->getGroupID() << 32) | val->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid = *temp >> 32;
            Nui32 pid = *temp & 0xffffffff;
            static_cast<MultiEditbox *>(own)->setSelectView(N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectionBrushImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };

    class _EngineInner CmdForceVertScrollbar : public PropertyCmd
    {
    public:
        CmdForceVertScrollbar() : PropertyCmd(N_PropertyUIMultiLineEditbox_VScrollAlwaysShow, _T("force_vert_scrollbar"),
            _T("Property to get/set the 'always show' setting for the vertical scroll bar of the list box."
            "Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const MultiEditbox *>(own)->isForceVScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<MultiEditbox *>(own)->SetForceVScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const MultiEditbox *>(own)->isForceVScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<MultiEditbox *>(own)->SetForceVScrollbar(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdForceVertScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    /*************************************************************************
        TODO:

        Clipboard support
        Undo support
    *************************************************************************/
    //------------------------------------------------------------------------
    NCount mle_getNextTokenLength(const String & text, NCount start_idx)
    {
        String::size_type pos = text.find_first_of(StrUtil::Delimiter, start_idx);

        // handle case where no more whitespace exists (so this is last token)
        if (pos == String::npos)
        {
            return (text.length() - start_idx);
        }
        // handle 'delimiter' token cases
        else if ((pos - start_idx) == 0)
        {
            return 1;
        }
        else
        {
            return (pos - start_idx);
        }
    }
    //------------------------------------------------------------------------
    N_EVENT(Container, MultiEditbox, ReadOnlyModeEvent,             0);
    N_EVENT(Container, MultiEditbox, WordWrapModeEvent,             1);
    N_EVENT(Container, MultiEditbox, MaxLengthEvent,                2);
    N_EVENT(Container, MultiEditbox, InputPosEvent,                 3);
    N_EVENT(Container, MultiEditbox, SelectEvent,                   4);
    N_EVENT(Container, MultiEditbox, InputFullEvent,                5);
    N_EVENT(Container, MultiEditbox, VScrollModeEvent,              6);
    N_EVENT(Container, MultiEditbox, HScrollModeEvent,              7);
    N_EVENT(Container, MultiEditbox, EventCount,                    8);
    #define d_lineBreakChars _T("\n")
    //------------------------------------------------------------------------
    const WidgetID MultiEditbox::VScrollbarID(1); //"__auto_vscrollbar__"
    const WidgetID MultiEditbox::HScrollbarID(2); //"__auto_hscrollbar__"
    //------------------------------------------------------------------------
    MultiEditbox::MultiEditbox(WidgetID wid, FactoryID fid, Container * own, 
        const String & name, LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_MultiLineEditbox, lid),
        mReadOnly(false),
        mMaxLength(String().max_size()),
        mViewBrush(0),
        d_caretPos(0),
        mSelectBegin(0),
        mSelectEnd(0),
        mDrag(false),
        d_dragAnchorIdx(0),
        mWordWrap(true),
        d_widestExtent(0.0f),
        mForceVScroll(false),
        mForceHScroll(false)
    {
        notifySignal(MultiEditbox::EventCount);
        setupCmd(N_CmdObj_MultiLineEditbox);

        // override default and disable text parsing
        mTextLayout = false;
        // Since parsing is ever allowed in the editbox, ban the property too.
        setWrite(N_PropertyUIWidget_TextLayout, false);
    }
    //------------------------------------------------------------------------
    MultiEditbox::~MultiEditbox()
    {
    }
    //------------------------------------------------------------------------
    void MultiEditbox::initChild()
    {
        // create the component sub-widgets
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        vertScrollbar->bind(Widget::ShowEvent, this, (CommandObjFunctor)&MultiEditbox::VScrollbarPrc);

        vertScrollbar->bind(Widget::HideEvent, this, (CommandObjFunctor)&MultiEditbox::VScrollbarPrc);

        vertScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&MultiEditbox::ScrollPrc);

        horzScrollbar->bind(Scrollbar::CurrentChageEvent, this, (CommandObjFunctor)&MultiEditbox::ScrollPrc);

        formatText(true);
        layout();
    }
    //------------------------------------------------------------------------
    NCount MultiEditbox::getSelectBegin() const
    {
        return (mSelectBegin != mSelectEnd) ? mSelectBegin : d_caretPos;
    }
    //------------------------------------------------------------------------
    NCount MultiEditbox::getSelectEnd() const
    {
        return (mSelectBegin != mSelectEnd) ? mSelectEnd : d_caretPos;
    }
    //------------------------------------------------------------------------
    void MultiEditbox::setReadOnly(bool set)
    {
        if (mReadOnly != set)
        {
            mReadOnly = set;
            WidgetEventArgs args(this);
            onReadOnlyMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::setInput(NCount caret_pos)
    {
        if(caret_pos > getText().length() - 1)
        {
           caret_pos = getText().length() - 1;
        }

        // if new position is different
        if(d_caretPos != caret_pos)
        {
            d_caretPos = caret_pos;
            visibleChar(d_caretPos);

            // Trigger "caret moved" event
            WidgetEventArgs args(this);
            onInputPos(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::setSelect(Nidx start_pos, Nidx end_pos)
    {
        // ensure selection start point is within the valid range
        if(start_pos > getText().length() - 1)
        {
           start_pos = getText().length() - 1;
        }

        // ensure selection end point is within the valid range
        if(end_pos > getText().length() - 1)
        {
           end_pos = getText().length() - 1;
        }

        // ensure start is before end
        if(start_pos > end_pos)
        {
            NCount tmp = end_pos;
            end_pos = start_pos;
            start_pos = tmp;
        }

        // only change state if values are different.
        if((start_pos != mSelectBegin) || (end_pos != mSelectEnd))
        {
            // setup selection
            mSelectBegin = start_pos;
            mSelectEnd     = end_pos;

            // Trigger "selection changed" event
            WidgetEventArgs args(this);
            onTextSelect(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::setSelectBegin(NCount start_pos)
    {
        setSelect(start_pos,start_pos + this->getSelectLength());
    }
    //------------------------------------------------------------------------
    void MultiEditbox::setSelectLength(NCount length)
    {
        setSelect(this->getSelectBegin(),this->getSelectBegin() + length);
    }
    //------------------------------------------------------------------------
    void MultiEditbox::setMaxLength(NCount max_len)
    {
        if(mMaxLength != max_len)
        {
            mMaxLength = max_len;

            // Trigger max length changed event
            WidgetEventArgs args(this);
            onMaxLength(&args);

            // trim string
            if(getText().length() > mMaxLength)
            {
                String newText = getText();
                newText.resize(mMaxLength);
                setText(newText);

                onText(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::visibleChar(NCount charpos)
    {
        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        // calculate the location of the caret
        const Font * font = getFont();
        NCount caretLine = getCharLine(charpos);

        if (caretLine < mLineList.size())
        {
            Rectf textArea(getTextRenderArea());

            NCount caretLineIdx = charpos - mLineList[caretLine].mCharIndex;

            NIIf ypos = caretLine * font->getLineSpacing();
            NIIf xpos = font->getTextAdvance(getText().substr(mLineList[caretLine].mCharIndex, caretLineIdx));

            // adjust position for scroll bars
            xpos -= horzScrollbar->getCurrent();
            ypos -= vertScrollbar->getCurrent();

            // if caret is above window, scroll up
            if (ypos < 0)
            {
                vertScrollbar->setCurrent(vertScrollbar->getCurrent() + ypos);
            }
            // if caret is below the window, scroll down
            else if ((ypos += font->getLineSpacing()) > textArea.getHeight())
            {
                vertScrollbar->setCurrent(vertScrollbar->getCurrent() + (ypos - textArea.getHeight()) + font->getLineSpacing());
            }

            // if caret is left of the window, scroll left
            if (xpos < 0)
            {
                horzScrollbar->setCurrent(horzScrollbar->getCurrent() + xpos - 50);
            }
            // if caret is right of the window, scroll right
            else if (xpos > textArea.getWidth())
            {
                horzScrollbar->setCurrent(horzScrollbar->getCurrent() + (xpos - textArea.getWidth()) + 50);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::setWordWrap(bool set)
    {
        if (set != mWordWrap)
        {
            mWordWrap = set;
            formatText(true);

            WidgetEventArgs args(this);
            onWordWrapMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::updateScrollbar()
    {
        Scrollbar * const vertScrollbar = getVScrollbar();
        Scrollbar * const horzScrollbar = getHScrollbar();
        const NIIf lspc = getFont()->getLineSpacing();

        //
        // First show or hide the scroll bars as needed (or requested)
        //
        // show or hide vertical scroll bar as required (or as specified by option)
        if (mForceVScroll || (static_cast<NIIf>(mLineList.size()) * lspc > getTextRenderArea().getHeight()))
        {
            vertScrollbar->show();

            // show or hide horizontal scroll bar as required (or as specified by option)
            horzScrollbar->setVisible(mForceHScroll || (d_widestExtent > getTextRenderArea().getWidth()));
        }
        // show or hide horizontal scroll bar as required (or as specified by option)
        else if (mForceHScroll || (d_widestExtent > getTextRenderArea().getWidth()))
        {
            horzScrollbar->show();

            // show or hide vertical scroll bar as required (or as specified by option)
            vertScrollbar->setVisible(mForceVScroll ||
                (static_cast<NIIf>(mLineList.size()) * lspc > getTextRenderArea().getHeight()));
        }
        else
        {
            vertScrollbar->hide();
            horzScrollbar->hide();
        }
        
        Rectf renderArea(getTextRenderArea());

        vertScrollbar->setDocSize(static_cast<NIIf>(mLineList.size()) * lspc);
        vertScrollbar->setPageSize(renderArea.height());
        vertScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderArea.height() / 10.0f));
        vertScrollbar->setCurrent(vertScrollbar->getCurrent());

        horzScrollbar->setDocSize(d_widestExtent);
        horzScrollbar->setPageSize(renderArea.width());
        horzScrollbar->setStepSize(N_MAX(NIIf, 1.0f, renderArea.width() / 10.0f));
        horzScrollbar->setCurrent(horzScrollbar->getCurrent());
    }
    //------------------------------------------------------------------------
    void MultiEditbox::formatText(bool udpateview)
    {
        // TODO: ASSAF - todo
        // clear old formatting data
        mLineList.clear();
        d_widestExtent = 0.0f;

        String paraText;

        const Font * font = getFont();

        if (font)
        {
            NIIf areaWidth = getTextRenderArea().getWidth();

            String::size_type    currPos = 0;
            String::size_type    paraLen;
            LineInfo    line;

            while (currPos < getText().length())
            {
               if ((paraLen = getText().find_first_of(d_lineBreakChars, currPos)) == String::npos)
                {
                   paraLen = getText().length() - currPos;
                }
                else
                {
                    ++paraLen -= currPos;
                }

                paraText = getText().substr(currPos, paraLen);

                if (!mWordWrap || (areaWidth <= 0.0f))
                {
                    // no word wrapping, so we are just one long line.
                    line.mCharIndex = currPos;
                    line.mLength    = paraLen;
                    line.mWidth    = font->getTextExtent(paraText);
                    mLineList.push_back(line);

                    // update widest, if needed.
                    if (line.mWidth > d_widestExtent)
                    {
                        d_widestExtent = line.mWidth;
                    }

                }
                // must word-wrap the paragraph text
                else
                {
                    String::size_type lineIndex = 0;

                    // while there is text in the string
                    while (lineIndex < paraLen)
                    {
                        String::size_type  lineLen = 0;
                        NIIf lineExtent = 0.0f;

                        // loop while we have not reached the end of the paragraph string
                        while (lineLen < (paraLen - lineIndex))
                        {
                            // get cp / char count of next token
                            NCount nextTokenSize = mle_getNextTokenLength(paraText, lineIndex + lineLen);

                            // get pixel width of the token
                            NIIf tokenExtent  = font->getTextExtent(paraText.substr(lineIndex + lineLen, nextTokenSize));

                            // would adding this token would overflow the available width
                            if ((lineExtent + tokenExtent) > areaWidth)
                            {
                                // Was this the first token?
                                if (lineLen == 0)
                                {
                                    // get point at which to break the token
                                    lineLen = font->getCharAtPixel(paraText.substr(lineIndex, nextTokenSize), areaWidth);
                                }

                                // text wraps, exit loop early with line info up until wrap point
                                break;
                            }

                            // add this token to current line
                            lineLen    += nextTokenSize;
                            lineExtent += tokenExtent;
                        }

                        // set up line info and add to collection
                        line.mCharIndex = currPos + lineIndex;
                        line.mLength    = lineLen;
                        line.mWidth    = lineExtent;
                        mLineList.push_back(line);

                        // update widest, if needed.
                        if (lineExtent > d_widestExtent)
                        {
                            d_widestExtent = lineExtent;
                        }

                        // update position in string
                        lineIndex += lineLen;
                    }
                }
                // skip to next 'paragraph' in text
                currPos += paraLen;
            }
        }

        if (udpateview)
        {
            updateScrollbar();
        }
        refresh();
    }
    //------------------------------------------------------------------------
    NCount MultiEditbox::getTextIndex(const Vector2f & pt) const
    {
        //
        // calculate final window position to be checked
        //
        Vector2f wndPt(pt.x - getActualPosX(), pt.y - getActualPosY());
        Rectf textArea(getTextRenderArea());

        wndPt -= textArea.min();

        // factor in scroll bar values
        wndPt.x += getHScrollbar()->getCurrent();
        wndPt.y += getVScrollbar()->getCurrent();

        NCount lineNumber = static_cast<NCount>(N_MAX(NIIf, 0.0f, wndPt.y) / getFont()->getLineSpacing());

        if(lineNumber >= mLineList.size())
        {
            lineNumber = mLineList.size() - 1;
        }

        String lineText(getText().substr(mLineList[lineNumber].mCharIndex, mLineList[lineNumber].mLength));

        NCount lineIdx = getFont()->getCharAtPixel(lineText, wndPt.x);

        if(lineIdx >= lineText.length() - 1)
        {
            lineIdx = lineText.length() - 1;
        }

        return mLineList[lineNumber].mCharIndex + lineIdx;
    }
    //------------------------------------------------------------------------
    NCount MultiEditbox::getCharLine(NCount index) const
    {
        NCount lineCount = mLineList.size();

        if(lineCount == 0)
        {
            return 0;
        }
        else if(index >= getText().length() - 1)
        {
            return lineCount - 1;
        }
        else
        {
            NCount indexCount = 0;
            NCount caretLine = 0;

            for(; caretLine < lineCount; ++caretLine)
            {
                indexCount += mLineList[caretLine].mLength;

                if(index < indexCount)
                {
                    return caretLine;
                }

            }

        }

        N_EXCEPT(InvalidRequest, _I18n("Unable to identify a line from the given, invalid, index."));
    }
    //------------------------------------------------------------------------
    void MultiEditbox::removeSelectText(bool modify)
    {
        if (getSelectLength() != 0)
        {
            setInput(getSelectBegin());

            // erase the selected characters (if required)
            if (modify)
            {
                String newText = getText();
                newText.erase(getSelectBegin(), getSelectLength());
                setText(newText);

                // trigger notification that text has changed.
                WidgetEventArgs args(this);
                onText(&args);
            }

            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    bool MultiEditbox::onCopyData(Clipboard & clipboard)
    {
        if (getSelectLength() == 0)
            return false;

        const String selectedText = getText().substr( getSelectBegin(), getSelectLength());

        clipboard.setText(selectedText);
        return true;
    }
    //------------------------------------------------------------------------
    bool MultiEditbox::onCutData(Clipboard & clipboard)
    {
        if (isReadOnly())
            return false;

        if (!onCopyData(clipboard))
            return false;

        onDeleteKey();
        return true;
    }
    //------------------------------------------------------------------------
    bool MultiEditbox::onPasteData(Clipboard & clipboard)
    {
        if (isReadOnly())
            return false;

        String clipboardText = clipboard.getText();

        if (clipboardText.empty())
            return false;

        // backup current text
        String tmp(getText());
        tmp.erase(getSelectBegin(), getSelectLength());

        removeSelectText();

        // if there is room
        if(getText().length() - clipboardText.length() < mMaxLength)
        {
            String newText = getText();
            newText.insert(getInput(), clipboardText);
            setText(newText);

            d_caretPos += clipboardText.length();

            WidgetEventArgs args(this);
            onText(&args);

            return true;
        }
        else
        {
            // Trigger text box full event
            WidgetEventArgs args(this);
            onTextFull(&args);

            return false;
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onBackspaceKey()
    {
        if (!isReadOnly())
        {
            if (getSelectLength() != 0)
            {
                removeSelectText();
            }
            else if (d_caretPos > 0)
            {
                String newText = getText();
                newText.erase(d_caretPos - 1, 1);
                setInput(d_caretPos - 1);
                setText(newText);

                WidgetEventArgs args(this);
                onText(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onDeleteKey()
    {
        if(!isReadOnly())
        {
            if(getSelectLength() != 0)
            {
                removeSelectText();
            }
            else if(getInput() < getText().length() - 1)
            {
                String newText = getText();
                newText.erase(d_caretPos, 1);
                setText(newText);

                visibleChar(d_caretPos);

                WidgetEventArgs args(this);
                onText(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onCharLeft(Nui32 sysKeys)
    {
        if (d_caretPos > 0)
        {
            setInput(d_caretPos - 1);
        }

        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onWordLeft(Nui32 sysKeys)
    {
        if (d_caretPos > 0)
        {
            setInput(StrUtil::getWordBegin(getText(), getInput()));
        }

        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onCharRight(Nui32 sysKeys)
    {
       if (d_caretPos < getText().length() - 1)
        {
            setInput(d_caretPos + 1);
        }

        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onWordRight(Nui32 sysKeys)
    {
       if (d_caretPos < getText().length() - 1)
        {
            setInput(StrUtil::getNextWordBegin(getText(), getInput()));
        }

        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onDocHome(Nui32 sysKeys)
    {
        if (d_caretPos > 0)
        {
            setInput(0);
        }

        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onDocEnd(Nui32 sysKeys)
    {
       if (d_caretPos < getText().length() - 1)
        {
           setInput(getText().length() - 1);
        }

        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onLineHome(Nui32 sysKeys)
    {
        NCount line = getCharLine(d_caretPos);

        if (line < mLineList.size())
        {
            NCount lineStartIdx = mLineList[line].mCharIndex;

            if (d_caretPos > lineStartIdx)
            {
                setInput(lineStartIdx);
            }

            if (sysKeys & WCK_KT_A_Shift)
            {
                setSelect(d_caretPos, d_dragAnchorIdx);
            }
            else
            {
                if (getSelectLength() != 0)
                {
                    setSelect(0, 0);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onLineEnd(Nui32 sysKeys)
    {
        NCount line = getCharLine(d_caretPos);

        if (line < mLineList.size())
        {
            NCount lineEndIdx = mLineList[line].mCharIndex + mLineList[line].mLength - 1;

            if (d_caretPos < lineEndIdx)
            {
                setInput(lineEndIdx);
            }

            if (sysKeys & WCK_KT_A_Shift)
            {
                setSelect(d_caretPos, d_dragAnchorIdx);
            }
            else
            {
                if (getSelectLength() != 0)
                {
                    setSelect(0, 0);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onLineUp(Nui32 sysKeys)
    {
        NCount caretLine = getCharLine(d_caretPos);

        if (caretLine > 0)
        {
            NIIf caretPixelOffset = getFont()->getTextAdvance(getText().substr(mLineList[caretLine].mCharIndex, d_caretPos - mLineList[caretLine].mCharIndex));

            --caretLine;

            NCount newLineIndex = getFont()->getCharAtPixel(getText().substr(mLineList[caretLine].mCharIndex, mLineList[caretLine].mLength), caretPixelOffset);

            setInput(mLineList[caretLine].mCharIndex + newLineIndex);
        }

        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onLineDown(Nui32 sysKeys)
    {
        NCount caretLine = getCharLine(d_caretPos);

        if ((mLineList.size() > 1) && (caretLine < (mLineList.size() - 1)))
        {
            NIIf caretPixelOffset = getFont()->getTextAdvance(getText().substr(mLineList[caretLine].mCharIndex, d_caretPos - mLineList[caretLine].mCharIndex));

            ++caretLine;

            NCount newLineIndex = getFont()->getCharAtPixel(getText().substr(mLineList[caretLine].mCharIndex, mLineList[caretLine].mLength), caretPixelOffset);

            setInput(mLineList[caretLine].mCharIndex + newLineIndex);
        }

        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onEnterKey(Nui32 /*sysKeys*/)
    {
        if (!isReadOnly())
        {
            removeSelectText();

            // if there is room
           if(getText().length() - 1 < mMaxLength)
            {
                String newText = getText();
                newText.insert(getInput(), 1, 0x0a);
                setText(newText);

                d_caretPos++;

                WidgetEventArgs args(this);
                onText(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onPageUp(Nui32 sysKeys)
    {
        NCount caretLine = getCharLine(d_caretPos);
        NCount nbLine = static_cast<NCount>(getTextRenderArea().getHeight() / getFont()->getLineSpacing());
        NCount newline = 0;
        if (nbLine < caretLine)
        {
            newline = caretLine - nbLine;
        }
        setInput(mLineList[newline].mCharIndex);
        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(d_caretPos, mSelectEnd);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
        visibleChar(d_caretPos);
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onPageDown(Nui32 sysKeys)
    {
        NCount caretLine = getCharLine(d_caretPos);
        NCount nbLine =  static_cast<NCount>(getTextRenderArea().getHeight() / getFont()->getLineSpacing());
        NCount newline = caretLine + nbLine;
        if (!mLineList.empty())
        {
            newline = std::min(newline,mLineList.size() - 1);
        }
        setInput(mLineList[newline].mCharIndex + mLineList[newline].mLength - 1);
        if (sysKeys & WCK_KT_A_Shift)
        {
            setSelect(mSelectBegin, d_caretPos);
        }
        else
        {
            if (getSelectLength() != 0)
            {
                setSelect(0, 0);
            }
        }
        visibleChar(d_caretPos);
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onButtonDown(const CursorEventArgs * arg)
    {
        // base class handling
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            // grab inputs
            if(focus())
            {
                if (getSelectLength() != 0)
                {
                    setSelect(0, 0);
                }
                mDrag = true;
                d_dragAnchorIdx = getTextIndex(
                    Vector2f(arg->mLocalX, arg->mLocalY));
                setInput(d_dragAnchorIdx);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onButtonUp(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onButtonUp(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            lost();
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onCursorMultiTouch(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onCursorMultiTouch(arg);

        if(arg->mCount == 2)
        {
            if(arg->mMouseArg.mButton == MB_Left)
            {
                d_dragAnchorIdx = StrUtil::getWordBegin(getText(), (d_caretPos == getText().length()) ? d_caretPos : d_caretPos + 1);
                d_caretPos      = StrUtil::getNextWordBegin(getText(), d_caretPos);

                // perform actual selection operation.
                setSelect(d_dragAnchorIdx, d_caretPos);
            }
        }
        else if(arg->mCount == 3)
        {
            if(arg->mMouseArg.mButton == MB_Left)
            {
                NCount caretLine = getCharLine(d_caretPos);
                NCount lineStart = mLineList[caretLine].mCharIndex;

                // find end of last paragraph
                String::size_type paraStart = getText().find_last_of(d_lineBreakChars, lineStart);

                // if no previous paragraph, selection will start at the beginning.
                if (paraStart == String::npos)
                {
                    paraStart = 0;
                }

                // find end of this paragraph
                String::size_type paraEnd = getText().find_first_of(d_lineBreakChars, lineStart);

                // if paragraph has no end, which actually should never happen, fix the
                // erroneous situation and select up to end at end of text.
                if (paraEnd == String::npos)
                {
                    String newText = getText();
                    newText.append(1, '\n');
                    setText(newText);

                    paraEnd = getText().length() - 1;
                }
                // set up selection using new values.
                d_dragAnchorIdx = paraStart;
                setInput(paraEnd);
                setSelect(d_dragAnchorIdx, d_caretPos);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onCursorMove(const CursorEventArgs * arg)
    {
        Widget::onCursorMove(arg);

        if(mDrag)
        {
            setInput(getTextIndex(Vector2f(arg->mLocalX, arg->mLocalY)));
            setSelect(d_caretPos, d_dragAnchorIdx);
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onLost(const WidgetEventArgs * arg)
    {
        mDrag = false;

        Widget::onLost(arg);
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onChar(const KeyEventArgs * arg)
    {
        // NB: We are not calling the base class handler here because it propogates
        // inputs back up the window hierarchy, whereas, as a consumer of key
        // events, we want such propogation to cease with us regardless of whether
        // we actually handle the event.

        signal(CharEvent, arg);

        // only need to take notice if we have focus
        if(isFinalActive() && !isReadOnly() && getFont()->isCodePointExist(arg->mCodePoint))
        {
            removeSelectText();

            // if there is room
            if(getText().length() - 1 < mMaxLength)
            {
               String newText = getText();
               newText.insert(getInput(), 1, arg->mCodePoint);
               setText(newText);

                d_caretPos++;

                WidgetEventArgs args(this);
                onText(&args);
            }
            else
            {
                WidgetEventArgs args(this);
                onTextFull(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onKeyDown(const KeyEventArgs * arg)
    {
        signal(KeyDownEvent, arg);

        if (isFinalActive())
        {
            Nmark wkc = mSheet->getWinComKey();
            if (isReadOnly())
            {
                Widget::onKeyDown(arg);
                return;
            }

            WidgetEventArgs args(this);
            if (arg->mScanCode == Key::LShift || arg->mScanCode == Key::RShift)
            {
                if (getSelectLength() == 0)
                {
                    d_dragAnchorIdx = getInput();
                }
            }
            else if (arg->mScanCode == Key::BK)
                onBackspaceKey();
            else if (arg->mScanCode == Key::M_Delete)
                onDeleteKey();
            else if (arg->mScanCode == Key::Enter || arg->mScanCode == Key::N_Enter)
                onEnterKey(wkc);
            else if (arg->mScanCode == Key::M_Left)
            {
                if (wkc & WCK_KT_A_Ctrl)
                {
                    onWordLeft(wkc);
                }
                else
                {
                    onCharLeft(wkc);
                }
            }
            else if (arg->mScanCode == Key::M_Right)
            {
                if (wkc & WCK_KT_A_Ctrl)
                {
                    onWordRight(wkc);
                }
                else
                {
                    onCharRight(wkc);
                }
            }
            else if (arg->mScanCode == Key::M_Up)
                onLineUp(wkc);
            else if (arg->mScanCode == Key::M_Down)
                onLineDown(wkc);
            else if (arg->mScanCode == Key::M_Home)
            {
                if (wkc & WCK_KT_A_Ctrl)
                {
                    onDocHome(wkc);
                }
                else
                {
                    onLineHome(wkc);
                }
            }
            else if (arg->mScanCode == Key::M_End)
            {
                if (wkc & WCK_KT_A_Ctrl)
                {
                    onDocEnd(wkc);
                }
                else
                {
                    onLineEnd(wkc);
                }
            }
            else if (arg->mScanCode == Key::M_PgUp)
                onPageUp(wkc);
            else if (arg->mScanCode == Key::M_PgDown)
                onPageDown(wkc);
            else
                Widget::onKeyDown(arg);
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onText(const WidgetEventArgs * arg)
    {
        // ensure last character is a new line
        if((getText().length() == 0) || (getText()[getText().length() - 1] != '\n'))
        {
            String newText = getText();
            newText.append(1, '\n');
            setText(newText);
        }

        // base class processing
        onText(arg);
        if (getSelectLength() != 0)
        {
            setSelect(0, 0);
        }
        // layout new text
        formatText(true);
        // layout child windows (scrollbars) since text layout may have changed
        layout();
        // ensure caret is still within the text
        setInput(getInput());
        // ensure caret is visible
        // NB: this will already have been called at least once, but since we
        // may have changed the formatting of the text, it needs to be called again.
        visibleChar(d_caretPos);
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onSize(const WidgetEventArgs * arg)
    {
        formatText(true);

        // base class handling
        Container::onSize(arg);
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onCursorDrag(const CursorEventArgs * arg)
    {
        // base class processing.
        Widget::onCursorDrag(arg);

        Scrollbar * vertScrollbar = getVScrollbar();
        Scrollbar * horzScrollbar = getHScrollbar();

        if(vertScrollbar->isFinalVisible() && (vertScrollbar->getDocSize() > vertScrollbar->getPageSize()))
        {
            vertScrollbar->setCurrent(vertScrollbar->getCurrent() + vertScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        }
        else if(horzScrollbar->isFinalVisible() && (horzScrollbar->getDocSize() > horzScrollbar->getPageSize()))
        {
            horzScrollbar->setCurrent(horzScrollbar->getCurrent() + horzScrollbar->getStepSize() * -arg->mMouseArg.mScroll);
        }
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onFont(const FontEventArgs * arg)
    {
        onFont(arg);
        formatText(true);
    }
    //------------------------------------------------------------------------
    bool MultiEditbox::validImpl(const WidgetView * renderer) const
    {
        return dynamic_cast<const MultiEditboxView *>(renderer) != 0;
    }
    //------------------------------------------------------------------------
    void MultiEditbox::onReadOnlyMode(const WidgetEventArgs * arg)
    {
        signal(ReadOnlyModeEvent, arg);

    }
    //------------------------------------------------------------------------
    void MultiEditbox::onWordWrapMode(const WidgetEventArgs * arg)
    {
        signal(WordWrapModeEvent, arg);

    }
    //------------------------------------------------------------------------
    void MultiEditbox::onMaxLength(const WidgetEventArgs * arg)
    {
        signal(MaxLengthEvent, arg);

    }
    //------------------------------------------------------------------------
    void MultiEditbox::onInputPos(const WidgetEventArgs * arg)
    {
        refresh();
        signal(InputPosEvent, arg);

    }
    //------------------------------------------------------------------------
    void MultiEditbox::onTextSelect(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SelectEvent, arg);

    }
    //------------------------------------------------------------------------
    void MultiEditbox::onTextFull(const WidgetEventArgs * arg)
    {
        signal(InputFullEvent, arg);

    }
    //------------------------------------------------------------------------
    void MultiEditbox::onVScrollbarMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(VScrollModeEvent, arg);

    }
    //------------------------------------------------------------------------
    void MultiEditbox::onHScrollbarMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(HScrollModeEvent, arg);

    }
    //------------------------------------------------------------------------
    bool MultiEditbox::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdReadOnly());
        dest->add(N_new CmdCaretIndex());
        dest->add(N_new CmdSelectionStart());
        dest->add(N_new CmdSelectionLength());
        dest->add(N_new CmdMaxTextLength());
        dest->add(N_new CmdWordWrap());
        dest->add(N_new CmdSelectionBrushImage());
        dest->add(N_new CmdForceVertScrollbar());
        return true;
    }
    //------------------------------------------------------------------------
    void MultiEditbox::ScrollPrc(const EventArgs *)
    {
        refresh();
    }
    //------------------------------------------------------------------------
    Scrollbar * MultiEditbox::getVScrollbar() const
    {
        return static_cast<Scrollbar *>(get((mID << N_Level_Bit_Escape) + VScrollbarID, false));
    }
    //------------------------------------------------------------------------
    Scrollbar * MultiEditbox::getHScrollbar() const
    {
        return static_cast<Scrollbar *>(get((mID << N_Level_Bit_Escape) + HScrollbarID, false));
    }
    //------------------------------------------------------------------------
    Rectf MultiEditbox::getTextRenderArea() const
    {
        if(mWidgetView != 0)
        {
            MultiEditboxView * wr = static_cast<MultiEditboxView*>(mWidgetView);
            return wr->getRenderArea();
        }

        N_EXCEPT(InvalidRequest, _I18n("This function must be implemented by the window renderer module"));
    }
    //--------------------------------------------------------------------------
    void MultiEditbox::setSelectView(PixelBuffer * pb)
    {
        mViewBrush = pb;
        refresh();
    }
    //------------------------------------------------------------------------
    void MultiEditbox::SetForceVScrollbar(bool b)
    {
        if(mForceVScroll != b)
        {
            mForceVScroll = b;

            updateScrollbar();
            WidgetEventArgs args(this);
            onVScrollbarMode(&args);
        }
    }
    //--------------------------------------------------------------------------
    void MultiEditbox::VScrollbarPrc(const EventArgs *)
    {
        if(mWordWrap)
            formatText(false);
    }
    //--------------------------------------------------------------------------
    /*
    Special:
            TextArea         - area where text, selection, and caret imagery will appear.
            TextAreaHScroll  - TextArea when only horizontal scrollbar is visible.
            TextAreaVScroll  - TextArea when only vertical scrollbar is visible.
            TextAreaHVScroll - TextArea when both horizontal and vertical scrollbar is visible.
    Imagery Sections:
            - Caret
    Child Widgets:
        Scrollbar based widget with name suffix "__auto_vscrollbar__"
        Scrollbar based widget with name suffix "__auto_hscrollbar__"
    */
    //--------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // MultiEditboxView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdBlinkCaret : public PropertyCmd
    {
    public:
        CmdBlinkCaret() : PropertyCmd(N_PropertyUIMultiLineEditbox_BlinkCaret, _T("BlinkCaret"),
            _T("Property to get/set whether the Editbox caret should blink.  "
            "Value is either \"True\" or \"False\"."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            MultiEditboxView * obj = static_cast<MultiEditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->isBlinkEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            MultiEditboxView * obj = static_cast<MultiEditboxView *>(static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setBlinkEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            MultiEditboxView * obj = static_cast<MultiEditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isBlinkEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            MultiEditboxView * obj = static_cast<MultiEditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)in;
            obj->setBlinkEnable(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBlinkCaret();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdBlinkCaretTimeout : public PropertyCmd
    {
    public:
        CmdBlinkCaretTimeout() : PropertyCmd(N_PropertyUIMultiLineEditbox_CaretBlinkTimeout, _T("BlinkCaretTimeout"),
            _T("Property to get/set the caret blink timeout / speed.  "
            "Value is a NIIf value indicating the timeout in seconds."))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            MultiEditboxView * obj = static_cast<MultiEditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->getBlinkTimeout(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            MultiEditboxView * obj = static_cast<MultiEditboxView *>(static_cast<Widget *>(own)->getWidgetView());
            Nui32 temp;
            StrConv::conv(val, temp);
            obj->setBlinkTimeout(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            MultiEditboxView * obj = static_cast<MultiEditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            Nui32 * temp = (Nui32 *)out;
            *temp = obj->getBlinkTimeout();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            MultiEditboxView * obj = static_cast<MultiEditboxView *>(static_cast<Widget *>(own)->getWidgetView());
            Nui32 * temp = (Nui32 *)in;
            obj->setBlinkTimeout(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdBlinkCaretTimeout();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIIf)0.66f, out);
        }
    };
    //------------------------------------------------------------------------
    N_STATE(WidgetView, MultiEditboxView, ReadOnlyState,        0);
    N_STATE(WidgetView, MultiEditboxView, ItemState,            1);
    N_STATE(WidgetView, MultiEditboxView, StateCount,           2);
    //------------------------------------------------------------------------
    #define DefaultCaretBlinkTimeout                            660
    //------------------------------------------------------------------------
    MultiEditboxView::MultiEditboxView(WidgetViewlID wmid) :
        WidgetView(wmid, N_VIEW_MultiLineEditbox),
        mBlinkTimeout(DefaultCaretBlinkTimeout),
        mBlinkCost(0),
        mBlinkEnable(false),
        mBlink(true)
    {
        add(N_new CmdBlinkCaret());
        add(N_new CmdBlinkCaretTimeout());
    }
    //------------------------------------------------------------------------
    Rectf MultiEditboxView::getRenderArea() const
    {
        MultiEditbox * w = (MultiEditbox *)mSrc;
        const Style * style = getStyle();
        bool v_visible = w->getVScrollbar()->isVisible();
        bool h_visible = w->getHScrollbar()->isVisible();
        Nid sid;

        // if either of the scrollbars are visible, we might want to use another text rendering area
        if (v_visible || h_visible)
        {
            if (h_visible)
            {
                if (v_visible)
                {
                    sid = TextAreaHVScroll;
                }
                else
                {
                    sid = TextAreaHScroll;
                }
            }
            else
            {
                if (v_visible)
                {
                    sid = TextAreaVScroll;
                }
                else
                {
                    sid = TextAreaScroll;
                }
            }

            if (style->isSpecialExist(sid))
            {
                return style->getSpecial(sid).getArea().getPixelRect(*w);
            }
        }

        // default to plain TextArea
        return style->getSpecial(TextArea).getArea().getPixelRect(*w);
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::onStyleAttach()
    {
        N_assert(mSrc != 0, "error logic");

        String text(mSrc->getText());
        if (text.empty() || text[text.length() - 1] != '\n')
        {
            text.append(1, '\n');
            mSrc->setText(text);
        }
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::flush()
    {
        MultiEditbox * w = (MultiEditbox *)mSrc;

        const Style * style = getStyle();
        // try and get mode for our current state
        const StyleLayout * ss = &style->getLayout(w->isFinalDisable() ?
            DisableState : (w->isReadOnly() ? ReadOnlyState : NormalState));
        ss->render(w);

        // Render edit box text
        Rectf textarea(getRenderArea());
        renderLineList(textarea);

        if ((w->isFinalActive() && !w->isReadOnly()) && (!mBlinkEnable || mBlink))
        {
            const Font * font = w->getFont();
            if (font)
            {
                NCount caretLine = w->getCharLine(w->getInput());

                const MultiEditbox::LineList& mLineList = w->getLineList();
                if (caretLine < mLineList.size())
                {
                    // calculate pixel offsets to where caret should be drawn
                    NCount caretLineIdx = w->getInput() - mLineList[caretLine].mCharIndex;
                    NIIf ypos = caretLine * font->getLineSpacing();
                    NIIf xpos = font->getTextAdvance(w->getText().substr(mLineList[caretLine].mCharIndex, caretLineIdx));

                    const StyleSection & caretImagery = style->getDesign(ItemState);

                    // calculate finat destination area for caret
                    Rectf caretArea;
                    caretArea.setLeft(textarea.left() + xpos);
                    caretArea.setTop(textarea.top() + ypos);
                    caretArea.setWidth(caretImagery.getBoundingRect(*w).getSize().mWidth);
                    caretArea.setHeight(font->getLineSpacing());
                    caretArea -= Vector2f(w->getHScrollbar()->getCurrent(), w->getVScrollbar()->getCurrent());

                    // cache the caret image for rendering.
                    caretImagery.render(w, caretArea, 0, &textarea);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::renderLineList(const Rectf & dest_area)
    {
        MultiEditbox * w = (MultiEditbox *)mSrc;
        Rectf drawArea(dest_area);
        NIIf vertScrollPos = w->getVScrollbar()->getCurrent();
        drawArea -= Vector2f(w->getHScrollbar()->getCurrent(), vertScrollPos);

        const Font * font = w->getFont();
        if (font)
        {
            ColourArea colours;
            ColourArea normalTextCol;
            ColourArea selectTextCol;
            
            const NIIf alpha = w->getAlphaEffect();
            getUnselectColour(normalTextCol);
            normalTextCol.setAlphaFactor(alpha);
            getSelectColour(selectTextCol);
            selectTextCol.setAlphaFactor(alpha);
            ColourArea selectBrushCol;
            w->isFinalActive() ? getActiveSelectColour(selectBrushCol) : getInactiveSelectColour(selectBrushCol);
            selectBrushCol.setAlphaFactor(alpha);

            const MultiEditbox::LineList & mLineList = w->getLineList();
            const NCount numLines = mLineList.size();

            // calculate the range of visible lines
            NCount sidx, eidx;
            sidx = static_cast<NCount>(vertScrollPos / font->getLineSpacing());
            eidx = 1 + sidx + static_cast<NCount>(dest_area.getHeight() / font->getLineSpacing());
            eidx = N_MIN(NCount, eidx, numLines);
            drawArea.mTop += font->getLineSpacing()*static_cast<NIIf>(sidx);

            // for each formatted line.
            for (NCount i = sidx; i < eidx; ++i)
            {
                Rectf lineRect(drawArea);
                const MultiEditbox::LineInfo& currLine = mLineList[i];
                String lineText(w->getVisualText().substr(currLine.mCharIndex, currLine.mLength));

                // oft the font little down so that it's centered within its own spacing
                const NIIf old_top = lineRect.top();
                lineRect.mTop += (font->getLineSpacing() - font->getFontHeight()) * 0.5f;

                // if it is a simple 'no selection area' case
                if ((currLine.mCharIndex >= w->getSelectEnd()) ||
                    ((currLine.mCharIndex + currLine.mLength) <= w->getSelectBegin()) ||
                    (w->getSelectView() == 0))
                {
                    colours = normalTextCol;
                    // render the complete line.
                    font->drawText(w->getPixelData(), lineText, lineRect.getPos(), &dest_area, colours);
                }
                // we have at least some selection highlighting to do
                else
                {
                    // Start of actual rendering section.
                    String sect;
                    NCount sectIdx = 0, sectLen;
                    NIIf selStartOffset = 0.0f, selAreaWidth = 0.0f;

                    // render any text prior to selected region of line.
                    if (currLine.mCharIndex < w->getSelectBegin())
                    {
                        // calculate length of text section
                        sectLen = w->getSelectBegin() - currLine.mCharIndex;

                        // get text for this section
                        sect = lineText.substr(sectIdx, sectLen);
                        sectIdx += sectLen;

                        // get the pixel oft to the beginning of the selection area highlight.
                        selStartOffset = font->getTextAdvance(sect);

                        // draw this portion of the text
                        colours = normalTextCol;
                        font->drawText(w->getPixelData(), sect, lineRect.getPos(), &dest_area, colours);

                        // set position ready for next portion of text
                        lineRect.mLeft += selStartOffset;
                    }

                    // calculate the length of the selected section
                    sectLen = N_MIN(NCount, w->getSelectEnd() - currLine.mCharIndex, currLine.mLength) - sectIdx;

                    // get the text for this section
                    sect = lineText.substr(sectIdx, sectLen);
                    sectIdx += sectLen;

                    // get the extent to use as the width of the selection area highlight
                    selAreaWidth = font->getTextAdvance(sect);

                    const NIIf text_top = lineRect.top();
                    lineRect.setTop(old_top);

                    // calculate area for the selection brush on this line
                    lineRect.setLeft(drawArea.left() + selStartOffset);
                    lineRect.setRight(lineRect.left() + selAreaWidth);
                    lineRect.setBottom(lineRect.top() + font->getLineSpacing());

                    // render the selection area brush for this line
                    colours = selectBrushCol;
                    w->getPixelData()->add(*w->getSelectView(), lineRect, &dest_area, colours);
                    // draw the text for this section
                    colours = selectTextCol;
                    font->drawText(w->getPixelData(), sect, lineRect.getPos(), &dest_area, colours);

                    lineRect.setTop(text_top);

                    // render any text beyond selected region of line
                    if (sectIdx < currLine.mLength)
                    {
                        // update render position to the end of the selected area.
                        lineRect.mLeft += selAreaWidth;

                        // calculate length of this section
                        sectLen = currLine.mLength - sectIdx;

                        // get the text for this section
                        sect = lineText.substr(sectIdx, sectLen);

                        // render the text for this section.
                        colours = normalTextCol;
                        font->drawText(w->getPixelData(), sect, lineRect.getPos(), &dest_area, colours);
                    }
                }

                // update master position for next line in paragraph.
                drawArea.mTop += font->getLineSpacing();
            }
        }
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::getUnselectColour(ColourArea & colour) const
    {
        getPropertyColour(N_PropertyUIMultiEditbox_UnselectTextColour, colour);
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::getSelectColour(ColourArea & colour) const
    {
        getPropertyColour(N_PropertyUIMultiEditbox_SelectTextColour, colour);
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::getActiveSelectColour(ColourArea & colour) const
    {
        getPropertyColour(N_PropertyUIMultiEditbox_ActiveSelectionColour, colour);
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::getInactiveSelectColour(ColourArea & colour) const
    {
        getPropertyColour(N_PropertyUIMultiEditbox_InactiveSelectionColour, colour);
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::getPropertyColour(PropertyID pid, ColourArea & colour) const
    {
        if (mSrc->isPropertyExist(pid))
            mSrc->get(pid, &colour);
        else
            colour.set(0);
    }
    //------------------------------------------------------------------------
    void MultiEditboxView::update(TimeDurMS cost)
    {
        // do base class stuff
        WidgetView::update(cost);

        MultiEditbox * tmp = static_cast<MultiEditbox *>(mSrc);
        // only do the update if we absolutely have to
        if (mBlinkEnable && !tmp->isReadOnly() && tmp->isFinalActive())
        {
            mBlinkCost += cost;

            if (mBlinkCost > mBlinkTimeout)
            {
                mBlinkCost = 0;
                mBlink ^= true;
                // state changed, so need a redraw
                mSrc->refresh();
            }
        }
    }
    //------------------------------------------------------------------------
    bool MultiEditboxView::notifyFont(const Font * font)
    {
        bool res = WidgetView::notifyFont(font);

        if (mSrc->getFont() == font)
        {
            mSrc->refresh();
            static_cast<MultiEditbox *>(mSrc)->formatText(true);
            return true;
        }

        return res;
    }
    //------------------------------------------------------------------------
}
}