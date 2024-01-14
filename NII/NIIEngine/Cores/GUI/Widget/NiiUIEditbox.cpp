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
#include "NiiUIEditbox.h"
#include "NiiUIException.h"
#include "NiiUIBidiText.h"
#include "NiiRegexManager.h"
#include "NiiKeyBoardControlPattern.h"
#include "NiiUIWidgetManager.h"
#include "NiiFont.h"

#define TextArea                            1
#define DefaultCaretBlinkTimeout            660

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdReadOnly : public PropertyCmd
    {
    public:
        CmdReadOnly() : PropertyCmd(N_PropertyUIEditbox_ReadOnly, _T("read_only"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Editbox *>(own)->isReadOnly(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Editbox *>(own)->setReadOnly(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Editbox *>(own)->isReadOnly();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Editbox *>(own)->setReadOnly(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdReadOnly();
        }
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdMaskText : public PropertyCmd
    {
    public:
        CmdMaskText() : PropertyCmd(N_PropertyUIEditbox_MarkMode, _T("mask_text"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Editbox *>(own)->isMarkMode(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Editbox *>(own)->setMarkMode(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Editbox *>(own)->isMarkMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Editbox *>(own)->setMarkMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaskText();
        }
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdMaskCodepoint : public PropertyCmd
    {
    public:
        CmdMaskCodepoint() : PropertyCmd(N_PropertyUIEditbox_MarkChar, _T("mask_codepoint"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Editbox *>(own)->getMarkChar(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Editbox *>(own)->setMarkChar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Editbox *>(own)->getMarkChar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Editbox *>(own)->setMarkChar(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaskCodepoint();
        }
        void getDefault(String & out) const
        {
            out = _T("*");
        }
    };

    class _EngineInner CmdValidationString : public PropertyCmd
    {
    public:
        CmdValidationString() : PropertyCmd(N_PropertyUIEditbox_RegexString, _T("validation_string"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const Editbox *>(own)->getRegexString();
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<Editbox *>(own)->setRegexString(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)temp;
            *temp = static_cast<const Editbox *>(own)->getRegexString();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            String * temp = (String *)in;
            static_cast<Editbox *>(own)->setRegexString(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdValidationString();
        }
        void getDefault(String & out) const
        {
            out = _T(".*");
        }
    };

    class _EngineInner CmdCaretIndex : public PropertyCmd
    {
    public:
        CmdCaretIndex() : PropertyCmd(N_PropertyUIEditbox_Input, _T("caret_index"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Editbox *>(own)->getInput(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Editbox *>(own)->setInput(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Editbox *>(own)->getInput();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Editbox *>(own)->setInput(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCaretIndex();
        }
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };

    class _EngineInner CmdSelectionStart : public PropertyCmd
    {
    public:
        CmdSelectionStart() : PropertyCmd(N_PropertyUIEditbox_SelectBegin, _T("selection_start"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Editbox *>(own)->getSelectBegin(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Editbox *>(own)->setSelectBegin(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Editbox *>(own)->getSelectBegin();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Editbox *>(own)->setSelectBegin(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectionStart();
        }
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };

    class _EngineInner CmdSelectionLength : public PropertyCmd
    {
    public:
        CmdSelectionLength() : PropertyCmd(N_PropertyUIEditbox_SelectLength, _T("selection_length"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Editbox *>(own)->getSelectLength(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Editbox *>(own)->setSelectLength(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Editbox *>(own)->getSelectLength();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Editbox *>(own)->setSelectLength(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectionLength();
        }
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };

    class _EngineInner CmdMaxTextLength : public PropertyCmd
    {
    public:
        CmdMaxTextLength() : PropertyCmd(N_PropertyUIEditbox_MaxLength, _T("max_text_length"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Editbox *>(own)->getMaxLength(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Editbox *>(own)->setMaxLength(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Editbox *>(own)->getMaxLength();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Editbox *>(own)->setMaxLength(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxTextLength();
        }
        void getDefault(String & out) const
        {
            StrConv::conv(out.max_size(), out);
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Widget,        Editbox,    ReadOnlyModeEvent,               0);
    N_EVENT(Widget,        Editbox,    MarkModeEvent,                   1);
    N_EVENT(Widget,        Editbox,    MarkCharChangeEvent,             2);
    N_EVENT(Widget,        Editbox,    RegexStringEvent,                3);
    N_EVENT(Widget,        Editbox,    MaxLengthEvent,                  4);
    N_EVENT(Widget,        Editbox,    TextValidChangeEvent,            5);
    N_EVENT(Widget,        Editbox,    InputPosEvent,                   6);
    N_EVENT(Widget,        Editbox,    SelectChangeEvent,               7);
    N_EVENT(Widget,        Editbox,    InputFullEvent,                  8);
    N_EVENT(Widget,        Editbox,    InputAcceptEvent,                9);
    N_EVENT(Widget,        Editbox,    EventCount,                      10);
    //------------------------------------------------------------------------
    Editbox::Editbox(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Widget(N_CmdObj_Editbox, wid, fid, own, name, lid),
        mMarkChar('*'),
        mMaxTextLength(String().max_size()),
        mInputPos(0),
        mSelectBegin(0),
        mSelectEnd(0),
        mCreateRegex(true),
        mReadOnly(false),
        mMarkMode(false),
        mDrag(false),
        mRegexState(RegexMatcher::MS_VALID),
        mPreStateChangeResponse(true)
    {
        notifySignal(Editbox::EventCount);
        setupCmd(N_CmdObj_Editbox);
        // override default and disable text parsing
        mTextLayout = false;
        // ban the property too, since this being off is not optional.
        setWrite(N_PropertyUIWidget_TextLayout, false);

        // default to accepting all characters
        if(mRegex)
            setRegexString(_T(".*"));
        // set copy of validation string to ".*" so getter returns something valid.
        else
            mRegexString = _T(".*");

        mRegex = N_Only(Regex).create();
    }
    //------------------------------------------------------------------------
    Editbox::~Editbox()
    {
        if(mCreateRegex && mRegex)
            N_Only(Regex).destroy(mRegex);
    }
    //------------------------------------------------------------------------
    NCount Editbox::getSelectBegin() const
    {
        return (mSelectBegin != mSelectEnd) ? mSelectBegin : mInputPos;
    }
    //------------------------------------------------------------------------
    NCount Editbox::getSelectEnd() const
    {
        return (mSelectBegin != mSelectEnd) ? mSelectEnd : mInputPos;
    }
    //------------------------------------------------------------------------
    void Editbox::setReadOnly(bool set)
    {
        // if setting is changed
        if (mReadOnly != set)
        {
            mReadOnly = set;
            WidgetEventArgs args(this);
            onReadOnlyMode(&args);
        }
    }
    //------------------------------------------------------------------------
    bool Editbox::isReadOnly() const
    {
        return mReadOnly;
    }
    //------------------------------------------------------------------------
    void Editbox::setMarkMode(bool set)
    {
        // if setting is changed
        if (mMarkMode != set)
        {
            mMarkMode = set;
            WidgetEventArgs args(this);
            onMarkMode(&args);
        }
    }
    //------------------------------------------------------------------------
    void Editbox::setRegexString(const String & string)
    {
        if (string == mRegexString || !mRegex)
            return;

        mRegexString = string;
        mRegex->setRegex(string);
        
        WidgetEventArgs args(this);
        onRegexString(&args);

        checkValid(getText());
    }
    //------------------------------------------------------------------------
    void Editbox::setInput(NCount caret_pos)
    {
        // make sure new position is valid
        if (caret_pos > getText().length())
            caret_pos = getText().length();

        // if new position is different
        if (mInputPos != caret_pos)
        {
            mInputPos = caret_pos;

            // Trigger "caret moved" event
            WidgetEventArgs args(this);
            onInputPos(&args);
        }
    }
    //------------------------------------------------------------------------
    NCount Editbox::getInput() const
    {
#ifdef CEGUI_BIDI_SUPPORT
        NCount caretPos = mInputPos;
        if(mBidiText->getLogicalToVisual().size() > caretPos)
            caretPos = mBidiText->getLogicalToVisual()[caretPos];
#endif
        return mInputPos;
    }
    //------------------------------------------------------------------------
    void Editbox::setSelect(NCount start_pos, NCount end_pos)
    {
        // ensure selection start point is within the valid range
        if(start_pos > getText().length())
            start_pos = getText().length();

        // ensure selection end point is within the valid range
        if(end_pos > getText().length())
            end_pos = getText().length();

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
            mSelectEnd  = end_pos;

            // Trigger "selection changed" event
            WidgetEventArgs args(this);
            onSelect(&args);
        }

    }
    //------------------------------------------------------------------------
    void Editbox::setSelectBegin(NCount start_pos)
    {
        this->setSelect(start_pos, start_pos + mSelectEnd - mSelectBegin);
    }
    //------------------------------------------------------------------------
    void Editbox::setSelectLength(NCount length)
    {
        this->setSelect(this->getSelectBegin(), this->getSelectBegin() + length);
    }
    //------------------------------------------------------------------------
    void Editbox::setMarkChar(Nui32 code)
    {
        if(code != mMarkChar)
        {
            mMarkChar = code;
            WidgetEventArgs args(this);
            onMarkChar(&args);
        }

    }
    //------------------------------------------------------------------------
    void Editbox::setMaxLength(NCount max_len)
    {
        if(mMaxTextLength != max_len)
        {
            mMaxTextLength = max_len;
            
            WidgetEventArgs args(this);
            onMaxLength(&args);

            // trim string
            if(getText().length() > mMaxTextLength)
            {
                String newText = getText();
                newText.resize(mMaxTextLength);
                setText(newText);
                onText(&args);

                MatchState state = getMatchState(getText());
                if(mRegexState != state)
                {
                    RegexMatchStateEventArgs rargs(this, state);
                    onTextValid(&rargs);
                    mRegexState = state;
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Editbox::removeSelectText(bool deletetext)
    {
        if(mSelectEnd - mSelectBegin != 0)
        {
            // setup new caret position and remove selection highlight.
            setInput(mSelectBegin);
            if(mSelectEnd - mSelectBegin != 0)
                setSelect(0, 0);

            if(deletetext)
            {
                String newText = getText();
                newText.erase(getSelectBegin(), mSelectEnd - mSelectBegin);
                setText(newText);

                // trigger notification that text has changed.
                WidgetEventArgs args(this);
                onText(&args);
            }
        }
    }
    //------------------------------------------------------------------------
    Editbox::MatchState Editbox::getMatchState(const String & str) const
    {
        return mRegex ? mRegex->match(str) : RegexMatcher::MS_VALID;
    }
    //------------------------------------------------------------------------
    void Editbox::setRegex(RegexMatcher * regex)
    {
        if(mCreateRegex && mRegex)
            N_Only(Regex).destroy(mRegex);

        mRegex = regex;

        if(mRegex)
            mCreateRegex = false;
        else
        {
            mRegex = N_Only(Regex).create();
            mCreateRegex = true;
        }
    }
    //------------------------------------------------------------------------
    bool Editbox::onCopyData(Clipboard & obj)
    {
        if(mSelectEnd - mSelectBegin == 0)
            return false;

        const String selectedText = getText().substr(getSelectBegin(), mSelectEnd - mSelectBegin);

        obj.setText(selectedText);
        return true;
    }
    //------------------------------------------------------------------------
    bool Editbox::onCutData(Clipboard & obj)
    {
        if(mReadOnly)
            return false;

        if(!onCopyData(obj))
            return false;

        onDeleteKey();
        return true;
    }
    //------------------------------------------------------------------------
    bool Editbox::onPasteData(Clipboard & obj)
    {
        if (mReadOnly)
            return false;

        String clipboardText = obj.getText();

        if (clipboardText.empty())
            return false;

        // backup current text
        String tmp(getText());
        tmp.erase(getSelectBegin(), mSelectEnd - mSelectBegin);

        // if there is room
        if (tmp.length() < mMaxTextLength)
        {
            tmp.insert(getSelectBegin(), clipboardText);

            if (checkValid(tmp))
            {
                // erase selection using mode that does not modify getText()
                // (we just want to update state)
                removeSelectText(false);

                // advance caret (done first so we can "do stuff" in event
                // handlers!)
                mInputPos += clipboardText.length();

                // set text to the newly modified string
                setText(tmp);

                return true;
            }
        }
        return false;
    }
    //------------------------------------------------------------------------
    bool Editbox::checkValid(const String & str)
    {
        MatchState n = getMatchState(str);

        if(n == mRegexState)
            return mPreStateChangeResponse;

        RegexMatchStateEventArgs args(this, n);
        onTextValid(&args);

        mRegexState = n;
        mPreStateChangeResponse = true;

        return true;
    }
    //------------------------------------------------------------------------
    void Editbox::onButtonDown(const CursorEventArgs * arg)
    {
        // base class handling
        Widget::onButtonDown(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            // grab inputs
            if(focus())
            {
                // handle mouse down
                if(mSelectEnd - mSelectBegin != 0)
                    setSelect(0, 0);
                mDrag = true;
                mDragBeginPos = getTextIndex(Vector2f(arg->mLocalX, arg->mLocalY));
#ifdef CEGUI_BIDI_SUPPORT
                if (mBidiText->getVisualToLogical().size() > mDragBeginPos)
                    mDragBeginPos = mBidiText->getVisualToLogical()[mDragBeginPos];
#endif
                setInput(mDragBeginPos);
            }
        }
    }
    //------------------------------------------------------------------------
    void Editbox::onButtonUp(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onButtonUp(arg);

        if(arg->mMouseArg.mButton == MB_Left)
        {
            lost();
        }
    }
    //------------------------------------------------------------------------
    void Editbox::onCursorMultiTouch(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onCursorMultiTouch(arg);

        if(arg->mCount == 2)
        {
            if(arg->mMouseArg.mButton == MB_Left)
            {
                // if masked, set up to select all
                if(mMarkMode)
                {
                    mDragBeginPos = 0;
                    setInput(getText().length());
                }
                // not masked, so select the word that was double-clicked.
                else
                {
                    mDragBeginPos = StrUtil::getWordBegin(getText(),
                        (mInputPos == getText().length()) ? mInputPos : mInputPos + 1);
                    mInputPos = StrUtil::getNextWordBegin(getText(), mInputPos);
                }

                // perform actual selection operation.
                setSelect(mDragBeginPos, mInputPos);
            }
        }
        else if(arg->mCount == 3)
        {
            if(arg->mMouseArg.mButton == MB_Left)
            {
                mDragBeginPos = 0;
                setInput(getText().length());
                setSelect(mDragBeginPos, mInputPos);
            }
        }
    }
    //------------------------------------------------------------------------
    void Editbox::onCursorMove(const CursorEventArgs * arg)
    {
        // base class processing
        Widget::onCursorMove(arg);

        if(mDrag)
        {
            NCount anchorIdx = getTextIndex(Vector2f(arg->mLocalX, arg->mLocalY));
#ifdef CEGUI_BIDI_SUPPORT
            if(mBidiText->getVisualToLogical().size() > anchorIdx)
                anchorIdx = mBidiText->getVisualToLogical()[anchorIdx];
#endif
            setInput(anchorIdx);
            setSelect(mInputPos, mDragBeginPos);
        }
    }
    //------------------------------------------------------------------------
    void Editbox::onLost(const WidgetEventArgs * arg)
    {
        mDrag = false;
        Widget::onLost(arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onChar(const KeyEventArgs * arg)
    {
        signal(CharEvent, arg);
        // only need to take notice if we have focus
        if(isFinalActive() && !mReadOnly && getFont()->isCodePointExist(arg->mCodePoint))
        {
            // backup current text
            String tmp(getText());
            tmp.erase(getSelectBegin(), mSelectEnd - mSelectBegin);

            // if there is room
            if(tmp.length() < mMaxTextLength)
            {
                tmp.insert(getSelectBegin(), 1, arg->mCodePoint);

                if(checkValid(tmp))
                {
                    // erase selection using mode that does not modify getText()
                    // (we just want to update state)
                    removeSelectText(false);

                    // advance caret (done first so we can "do stuff" in event
                    // handlers!)
                    mInputPos++;

                    // set text to the newly modified string
                    setText(tmp);

                    // char was accepted into the Editbox - mark event as handled.
                }
            }
            else
            {
                // Trigger text box full event
                WidgetEventArgs args(this);
                onTextFull(&args);
            }
        }
        // event was (possibly) not handled
    }
    //------------------------------------------------------------------------
    void Editbox::onKeyDown(const KeyEventArgs * arg)
    {
        // fire event.
        signal(KeyDownEvent, arg);
        Nmark wck = mSheet->getWinComKey();
        if(isFinalActive())
        {
            if(mReadOnly)
            {
                Widget::onKeyDown(arg);
                return;
            }

            WidgetEventArgs args(this);

            if(arg->mScanCode == Key::LShift || arg->mScanCode == Key::RShift)
                if (mSelectEnd - mSelectBegin == 0)
                    mDragBeginPos = mInputPos;
            else if (arg->mScanCode == Key::BK)
                onBackspaceKey();
            else if (arg->mScanCode == Key::M_Delete)
                onDeleteKey();
            else if (arg->mScanCode == Key::TAB || arg->mScanCode == Key::Enter ||
                arg->mScanCode == Key::N_Enter)
                // Fire 'input accepted' event
                onTextAccept(&args);
            else if (arg->mScanCode == Key::M_Left)
                if (wck & WCK_KT_A_Ctrl)
                    onWordLeft(wck);
                else
                    onCharLeft(wck);
            else if (arg->mScanCode == Key::M_Right)
                if (wck & WCK_KT_A_Ctrl)
                    onWordRight(wck);
                else
                    onCharRight(wck);
            else if (arg->mScanCode == Key::M_Home)
                onHomeKey(wck);
            else if (arg->mScanCode == Key::M_End)
                onEndKey(wck);
            else
                Widget::onKeyDown(arg);
        }
    }
    //------------------------------------------------------------------------
    void Editbox::onBackspaceKey()
    {
        if (!mReadOnly)
        {
            String tmp(getText());

            if (mSelectEnd - mSelectBegin != 0)
            {
                tmp.erase(getSelectBegin(), mSelectEnd - mSelectBegin);

                if (checkValid(tmp))
                {
                    // erase selection using mode that does not modify getText()
                    // (we just want to update state)
                    removeSelectText(false);

                    // set text to the newly modified string
                    setText(tmp);
                }
            }
            else if (getInput() > 0)
            {
                tmp.erase(mInputPos - 1, 1);

                if (checkValid(tmp))
                {
                    setInput(mInputPos - 1);

                    // set text to the newly modified string
                    setText(tmp);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Editbox::onDeleteKey()
    {
        if (!mReadOnly)
        {
            String tmp(getText());

            if (mSelectEnd - mSelectBegin != 0)
            {
                tmp.erase(getSelectBegin(), mSelectEnd - mSelectBegin);

                if (checkValid(tmp))
                {
                    // erase selection using mode that does not modify getText()
                    // (we just want to update state)
                    removeSelectText(false);

                    // set text to the newly modified string
                    setText(tmp);
                }
            }
            else if (getInput() < tmp.length())
            {
                tmp.erase(mInputPos, 1);

                if (checkValid(tmp))
                {
                    // set text to the newly modified string
                    setText(tmp);
                }
            }
        }
    }
    //------------------------------------------------------------------------
    void Editbox::onCharLeft(Nui32 sysKeys)
    {
        if (mInputPos > 0)
            setInput(mInputPos - 1);

        if (sysKeys & WCK_KT_A_Shift)
            setSelect(mInputPos, mDragBeginPos);
        else
            if (mSelectEnd - mSelectBegin != 0)
                setSelect(0, 0);
    }
    //------------------------------------------------------------------------
    void Editbox::onWordLeft(Nui32 sysKeys)
    {
        if (mInputPos > 0)
            setInput(StrUtil::getWordBegin(getText(), mInputPos));

        if (sysKeys & WCK_KT_A_Shift)
            setSelect(mInputPos, mDragBeginPos);
        else
            if (mSelectEnd - mSelectBegin != 0)
                setSelect(0, 0);
    }
    //------------------------------------------------------------------------
    void Editbox::onCharRight(Nui32 sysKeys)
    {
        if (mInputPos < getText().length())
            setInput(mInputPos + 1);

        if (sysKeys & WCK_KT_A_Shift)
            setSelect(mInputPos, mDragBeginPos);
        else
            if (mSelectEnd - mSelectBegin != 0)
                setSelect(0, 0);
    }
    //------------------------------------------------------------------------
    void Editbox::onWordRight(Nui32 sysKeys)
    {
        if (mInputPos < getText().length())
            setInput(StrUtil::getNextWordBegin(getText(), mInputPos));

        if (sysKeys & WCK_KT_A_Shift)
            setSelect(mInputPos, mDragBeginPos);
        else
            if (mSelectEnd - mSelectBegin != 0)
                setSelect(0, 0);
    }
    //------------------------------------------------------------------------
    void Editbox::onHomeKey(Nui32 sysKeys)
    {
        if (mInputPos > 0)
            setInput(0);

        if (sysKeys & WCK_KT_A_Shift)
            setSelect(mInputPos, mDragBeginPos);
        else
            if (mSelectEnd - mSelectBegin != 0)
                setSelect(0, 0);
    }
    //------------------------------------------------------------------------
    void Editbox::onEndKey(Nui32 sysKeys)
    {
        if (mInputPos < getText().length())
            setInput(getText().length());

        if (sysKeys & WCK_KT_A_Shift)
            setSelect(mInputPos, mDragBeginPos);
        else
            if (mSelectEnd - mSelectBegin != 0)
                setSelect(0, 0);
    }
    //------------------------------------------------------------------------
    bool Editbox::validImpl(const WidgetView * style) const
    {
        return dynamic_cast<const EditboxView *>(style) != 0;
    }
    //------------------------------------------------------------------------
    void Editbox::onReadOnlyMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(ReadOnlyModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onMarkMode(const WidgetEventArgs * arg)
    {
        refresh();
        signal(MarkModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onMarkChar(const WidgetEventArgs * arg)
    {
        // if we are in masked mode, trigger a GUI redraw.
        if(mMarkMode)
        {
            refresh();
        }
        signal(MarkCharChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onRegexString(const WidgetEventArgs * arg)
    {
        signal(RegexStringEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onMaxLength(const WidgetEventArgs * arg)
    {
        signal(MaxLengthEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onTextValid(const RegexMatchStateEventArgs * arg)
    {
        signal(TextValidChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onInputPos(const WidgetEventArgs * arg)
    {
        refresh();
        signal(InputPosEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onSelect(const WidgetEventArgs * arg)
    {
        refresh();
        signal(SelectChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onTextFull(const WidgetEventArgs * arg)
    {
        signal(InputFullEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onTextAccept(const WidgetEventArgs * arg)
    {
        signal(InputAcceptEvent, arg);
    }
    //------------------------------------------------------------------------
    void Editbox::onText(const WidgetEventArgs * arg)
    {
        // base class processing
        onText(arg);

        if(mSelectEnd - mSelectBegin != 0)
            setSelect(0, 0);
        // make sure caret is within the text
        if(mInputPos > getText().length())
            setInput(getText().length());
    }
    //------------------------------------------------------------------------
    bool Editbox::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdReadOnly());
        dest->add(N_new CmdMaskText());
        dest->add(N_new CmdMaskCodepoint());
        dest->add(N_new CmdValidationString());
        dest->add(N_new CmdCaretIndex());
        dest->add(N_new CmdSelectionStart());
        dest->add(N_new CmdSelectionLength());
        dest->add(N_new CmdMaxTextLength());
        return true;
    }
    //------------------------------------------------------------------------
    NCount Editbox::getTextIndex(const Vector2f & pt) const
    {
        if (mWidgetView != 0)
        {
            EditboxView * wr = (EditboxView *)mWidgetView;
            return wr->getTextIndex(pt);
        }
        else
        {
            N_EXCEPT(InvalidRequest, _I18n("This function must be implemented by the window renderer"));
        }
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // EditboxView
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdBlinkCaret : public PropertyCmd
    {
    public:
        CmdBlinkCaret() : PropertyCmd(N_PropertyUIEditbox_BlinkEnable, _T("BlinkCaret"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            EditboxView * obj = static_cast<EditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->isBlinkEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            EditboxView * obj = static_cast<EditboxView *>(static_cast<Widget *>(own)->getWidgetView());
            bool temp;
            StrConv::conv(val, temp);
            obj->setBlinkEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            EditboxView * obj = static_cast<EditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            bool * temp = (bool *)out;
            *temp = obj->isBlinkEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            EditboxView * obj = static_cast<EditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
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
        CmdBlinkCaretTimeout() : PropertyCmd(N_PropertyUIEditbox_BlinkTimeout, _T("BlinkCaretTimeout"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            EditboxView * obj = static_cast<EditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            StrConv::conv(obj->getBlinkTimeout(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            EditboxView * obj = static_cast<EditboxView *>(static_cast<Widget *>(own)->getWidgetView());
            Nui32 temp;
            StrConv::conv(val, temp);
            obj->setBlinkTimeout(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            EditboxView * obj = static_cast<EditboxView *>(static_cast<const Widget *>(own)->getWidgetView());
            Nui32 * temp = (Nui32 *)out;
            *temp = obj->getBlinkTimeout();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            EditboxView * obj = static_cast<EditboxView *>(static_cast<Widget *>(own)->getWidgetView());
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
            StrConv::conv(DefaultCaretBlinkTimeout, out);
        }
    };

    class _EngineInner CmdTextFormatting : public PropertyCmd
    {
    public:
        CmdTextFormatting() : PropertyCmd(N_PropertyUIEditbox_TextAlign, _T("TextFormatting"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            EditboxView * obj = static_cast<EditboxView *>(
                static_cast<const Widget *>(own)->getWidgetView());
            HTextLayout val = obj->getTextLayout();

            switch (val)
            {
            case HTL_Right:
                out = _T("right");
                break;
            case HTL_Centre:
                out = _T("centre");
                break;
            case HTL_Normal:
                out = _T("Justified");
                break;
            case HTL_Wrap_Left:
                out = _T("WordWrapLeftAligned");
                break;
            case HTL_Wrap_Right:
                out = _T("WordWrapRightAligned");
                break;
            case HTL_Wrap_Centre:
                out = _T("WordWrapCentreAligned");
                break;
            case HTL_Wrap_Expand:
                out = _T("WordWrapJustified");
                break;
            default:
                out = _T("left");
                break;
            }
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            EditboxView * obj = static_cast<EditboxView *>(
                static_cast<Widget *>(own)->getWidgetView());
            HTextLayout temp;
            if (val == _T("centre"))
                temp = HTL_Centre;
            else if (val == _T("right"))
                temp = HTL_Right;
            else if (val == _T("Justified"))
                temp = HTL_Normal;
            else if (val == _T("WordWrapLeftAligned"))
                temp = HTL_Wrap_Left;
            else if (val == _T("WordWrapCentreAligned"))
                temp = HTL_Wrap_Centre;
            else if (val == _T("WordWrapRightAligned"))
                temp = HTL_Wrap_Right;
            else if (val == _T("WordWrapJustified"))
                temp = HTL_Wrap_Expand;
            else
                temp = HTL_Left;
            obj->setTextLayout(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            EditboxView * obj = static_cast<EditboxView *>(
                static_cast<const Widget *>(own)->getWidgetView());
            HTextLayout * temp = (HTextLayout *)out;
            *temp = obj->getTextLayout();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            EditboxView * obj = static_cast<EditboxView *>(
                static_cast<Widget *>(own)->getWidgetView());
            HTextLayout * temp = (HTextLayout *)in;
            obj->setTextLayout(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTextFormatting();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("left");
        }
    };
    //------------------------------------------------------------------------
    /*
    Special:
            - TextArea: area where text, selection, and caret imagery will appear.

    PropertyDefinitions (optional)
            - NormalTextColour: property that accesses a colour value to be used to
                render normal unselected text.  If this property is
                not defined, the colour defaults to black.
            - SelectedTextColour: property that accesses a colour value to be used
                to render selected text.  If this property is
                not defined, the colour defaults to black.

    Imagery Sections:
            - Caret
    */
    //------------------------------------------------------------------------
    N_STATE(WidgetView, EditboxView, ReadOnlyState,       0);
    N_STATE(WidgetView, EditboxView, SelectColourState,   1);
    N_STATE(WidgetView, EditboxView, UnselectColourState, 2);
    N_STATE(WidgetView, EditboxView, ItemState,           3);
    N_STATE(WidgetView, EditboxView, StateCount,          4);
    //------------------------------------------------------------------------
    EditboxView::EditboxView(WidgetViewlID wsid) :
        WidgetView(wsid, N_WIDGET_Editbox),
        mTextOffset(0.0f),
        mBlinkTimeout(DefaultCaretBlinkTimeout),
        mBlinkCost(0),
        mTextLayout(HTL_Left),
        mBlinkEnable(false),
        mBlink(true)
    {
        add(N_new CmdBlinkCaret());
        add(N_new CmdBlinkCaretTimeout());
        add(N_new CmdTextFormatting());
    }
    //------------------------------------------------------------------------
    void EditboxView::flush()
    {
        const Style * style = getStyle();

        Editbox * w = static_cast<Editbox *>(mSrc);

        const StyleLayout * slayout = &style->getLayout(w->isFinalDisable() ? DisableState :
            (w->isReadOnly() ? ReadOnlyState : NormalState));

        slayout->render(w);

        const Font * font = mSrc->getFont();
        if (!font)
            return;

        String visual_text;
        if (w->isMarkMode())
            visual_text.assign(w->getText().length(), w->getMarkChar());
        else
            visual_text.assign(w->getVisualText());

        const StyleSection & ssection = style->getDesign(ItemState);

        // get destination area for text
        const Rectf text_area(style->getSpecial(TextArea).getArea().getPixelRect(*mSrc));

        const NCount caret_index = getInput(visual_text);
        const NIIf extent_to_caret = font->getTextAdvance(visual_text.substr(0, caret_index));
        const NIIf caret_width = ssection.getBoundingRect(*mSrc, text_area).getWidth();
        const NIIf text_extent = font->getTextExtent(visual_text);
        const NIIf text_offset = calcTextOffset(text_area, text_extent, caret_width, extent_to_caret);

#ifdef CEGUI_BIDI_SUPPORT
        renderTextBidi(style, visual_text, text_area, text_offset);
#else
        renderTextNoBidi(*style, visual_text, text_area, text_offset);
#endif
        // remember this for next time.
        mTextOffset = text_offset;

        render(ssection, text_area, text_offset, extent_to_caret);
    }
    //------------------------------------------------------------------------
    NCount EditboxView::getInput(const String & visual_text) const
    {
        Editbox * w = static_cast<Editbox *>(mSrc);

        NCount caretIndex = w->getInput();

#ifdef CEGUI_BIDI_SUPPORT
        // the char before the caret bidi type
        bool currCharIsRtl = false;
        if (!visual_text.empty() && caretIndex > 0)
        {
            NCount curCaretIndex = w->getInput();
            BidiType charBeforeCaretType = w->getBidiText()->getBidiType(visual_text[curCaretIndex - 1]);
            // for neutral chars you decide by the char after
            for (; BT_Normal == charBeforeCaretType &&
                (visual_text.size() > curCaretIndex); curCaretIndex++)
            {
                charBeforeCaretType = w->getBidiText()->getBidiType(visual_text[curCaretIndex - 1]);
            }

            currCharIsRtl = (BT_RTL == charBeforeCaretType);
        }

        const bool isFirstChar = caretIndex == 0;

        // the pos is by the char before
        if (!isFirstChar)
            caretIndex--;

        // we need to find the caret pos by the logical to visual map
        if (w->getBidiText()->getVisualToLogical().size() > caretIndex)
            caretIndex = w->getBidiText()->getLogicalToVisual()[caretIndex];

        // for non RTL char - the caret pos is after the char
        if (!currCharIsRtl)
            caretIndex++;

        // if first char is not rtl - we need to stand at the start of the line
        if (isFirstChar)
        {
            bool firstCharRtl = !visual_text.empty() &&
                (BT_RTL == w->getBidiText()->getBidiType(visual_text[0]));

            if (!firstCharRtl)
                caretIndex--;
        }
#endif

        return caretIndex;
    }
    //------------------------------------------------------------------------
    NIIf EditboxView::calcTextOffset(const Rectf & text_area,
        const NIIf text_extent, const NIIf caret_width, const NIIf extent_to_caret)
    {
        // if caret is to the left of the box
        if ((mTextOffset + extent_to_caret) < 0)
            return -extent_to_caret;

        // if caret is off to the right.
        if ((mTextOffset + extent_to_caret) >= (text_area.getWidth() - caret_width))
            return text_area.getWidth() - extent_to_caret - caret_width;

        // handle formatting of text when it's shorter than the available space
        if (text_extent < text_area.getWidth())
        {
            if (mTextLayout == HTL_Centre)
                return (text_area.getWidth() - text_extent) / 2;

            if (mTextLayout == HTL_Right)
                return text_area.getWidth() - text_extent;
        }

        // no change to text position; re-use last offset value.
        return mTextOffset;
    }
    //------------------------------------------------------------------------
    void EditboxView::renderTextNoBidi(const Style & style,
        const String & text, const Rectf & text_area, NIIf text_offset)
    {
        const Font * font = mSrc->getFont();

        // setup initial rect for text formatting
        Rectf text_part_rect(text_area);
        // allow for scroll position
        text_part_rect.mLeft += text_offset;
        // centre text vertically within the defined text area
        text_part_rect.mTop += (text_area.getHeight() - font->getFontHeight()) * 0.5f;

        ColourArea colours;
        const NIIf alpha_comp = mSrc->getAlphaEffect();
        // get unhighlighted text colour (saves accessing property twice)
        ColourArea unselectedColours;
        getUnselectColour(unselectedColours);
        // see if the editbox is active or inactive.
        Editbox * const w = static_cast<Editbox *>(mSrc);
        const bool active = w->isFinalActive();

        if (w->getSelectLength() != 0)
        {
            // calculate required start and end offsets of selection mode.
            NIIf selStartOffset = font->getTextAdvance(text.substr(0, w->getSelectBegin()));
            NIIf selEndOffset = font->getTextAdvance(text.substr(0, w->getSelectEnd()));

            // calculate area for selection mode.
            Rectf hlarea(text_area);
            hlarea.mLeft += text_offset + selStartOffset;
            hlarea.mRight = hlarea.mLeft + (selEndOffset - selStartOffset);

            style.getLayout(active ? SelectColourState : UnselectColourState).render(w, hlarea, 0, &text_area);
        }

        // draw pre-highlight text
        String sect = text.substr(0, w->getSelectBegin());
        colours = unselectedColours;
        colours.setAlphaFactor(alpha_comp);
        text_part_rect.mLeft = font->drawText(w->getPixelData(), sect, text_part_rect.getPos(), &text_area, colours);

        // draw highlight text
        sect = text.substr(w->getSelectBegin(), w->getSelectLength());
        getSelectColour(colours);
        colours.setAlphaFactor(alpha_comp);
        text_part_rect.mLeft = font->drawText(w->getPixelData(), sect, text_part_rect.getPos(), &text_area, colours);

        // draw post-highlight text
        sect = text.substr(w->getSelectEnd());
        colours = unselectedColours;
        colours.setAlphaFactor(alpha_comp);
        font->drawText(w->getPixelData(), sect, text_part_rect.getPos(), &text_area, colours);
    }
    //------------------------------------------------------------------------
    void EditboxView::renderTextBidi(const Style & style,
        const String & text, const Rectf & text_area, NIIf text_offset)
    {
#ifdef CEGUI_BIDI_SUPPORT
        const Font * const font = mSrc->getFont();

        // setup initial rect for text formatting
        Rectf text_part_rect(text_area);
        // allow for scroll position
        text_part_rect.mLeft += text_offset;
        // centre text vertically within the defined text area
        text_part_rect.mTop += (text_area.getHeight() - font->getFontHeight()) * 0.5f;

        ColourArea colours;
        const NIIf alpha_comp = mSrc->getAlphaEffect();
        // get unhighlighted text colour (saves accessing property twice)
        ColourArea unselectedColour;
        getUnselectColour(unselectedColour);
        // see if the editbox is active or inactive.
        Editbox * const w = static_cast<Editbox *>(mSrc);
        const bool active = w->isFinalActive();

        if (w->getSelectLength() == 0)
        {
            // no highlighted text - we can draw the whole thing
            colours = unselectedColour;
            colours.setAlphaFactor(alpha_comp);
            text_part_rect.mLeft =
                font->drawText(w->getPixelData(), text,
                    text_part_rect.getPos(), &text_area, colours);
        }
        else
        {
            // there is highlighted text - because of the Bidi support - the
            // highlighted area can be in some cases nonconsecutive.
            // So - we need to draw it char by char (I guess we can optimize it more
            // but this is not that big performance hit because it only happens if
            // we have highlighted text - not that common...)
            for (NCount i = 0; i < text.size(); ++i)
            {
                // get the char
                String currChar = text.substr(i, 1);
                NCount realPos = 0;

                // get he visual pos of the char
                if (w->getBidiText()->getVisualToLogical().size() > i)
                {
                    realPos = w->getBidiText()->getVisualToLogical()[i];
                }

                // check if it is in the highlighted region
                bool highlighted =
                    realPos >= w->getSelectBegin() &&
                    realPos < w->getSelectBegin() + w->getSelectLength();

                NIIf charAdvance = font->getCodePoint(currChar[0])->getHoriAdvance();

                if (highlighted)
                {
                    getSelectColour(colours);
                    colours.setAlphaFactor(alpha_comp);

                    {
                        // calculate area for selection mode.
                        Rectf hlarea(text_area);
                        hlarea.mLeft = text_part_rect.mLeft;
                        hlarea.mRight = text_part_rect.mLeft + charAdvance;

                        style.getLayout(active ? SelectColourState : UnselectColourState).render(*w, hlarea, 0, &text_area);
                    }
                }
                else
                {
                    colours = unselectedColour;
                    colours.setAlphaFactor(alpha_comp);
                }
                font->drawText(w->getPixelData(), currChar, text_part_rect.getPos(), &text_area, colours);

                // adjust rect for next section
                text_part_rect.mLeft += charAdvance;
            }
        }
#endif
    }
    //------------------------------------------------------------------------
    void EditboxView::render(const StyleSection & mode,
        const Rectf & text_area, NIIf text_offset, NIIf extent_to_caret) const
    {
        Editbox * const we = static_cast<Editbox *>(mSrc);
        if ((!mBlinkEnable || mBlink) && we->isFinalActive() && !we->isReadOnly())
        {
            Rectf caretRect(text_area);
            caretRect.mLeft += extent_to_caret + text_offset;

            mode.render(mSrc, caretRect, 0, &text_area);
        }
    }
    //------------------------------------------------------------------------
    NCount EditboxView::getTextIndex(const Vector2f & pt) const
    {
        Editbox * w = static_cast<Editbox *>(mSrc);

        // calculate final window position to be checked
        NIIf wndx = pt.x - w->getActualPosX();;

        wndx -= mTextOffset;

        if (w->isMarkMode())
            return w->getFont()->getCharAtPixel(String(w->getVisualText().length(), w->getMarkChar()), wndx);
        else
            return w->getFont()->getCharAtPixel(w->getVisualText(), wndx);
    }
    //------------------------------------------------------------------------
    void EditboxView::getUnselectColour(ColourArea & colour) const
    {
        getPropertyColour(N_PropertyUIEditbox_UnselectTextColour, colour);
    }
    //------------------------------------------------------------------------
    void EditboxView::getSelectColour(ColourArea & colour) const
    {
        getPropertyColour(N_PropertyUIEditbox_SelectTextColour, colour);
    }
    //------------------------------------------------------------------------
    void EditboxView::getPropertyColour(PropertyID pid, ColourArea & out) const
    {
        if (mSrc->isPropertyExist(pid))
        {
            String str;
            mSrc->get(pid, &out);
        }
        else
        {
            out.set(0);
        }
    }
    //------------------------------------------------------------------------
    void EditboxView::update(TimeDurMS cost)
    {
        WidgetView::update(cost);
        Editbox * w = static_cast<Editbox *>(mSrc);
        // only do the update if we absolutely have to
        if (mBlinkEnable && !w->isReadOnly() && w->isFinalActive())
        {
            mBlinkCost += cost;

            if (mBlinkCost > mBlinkTimeout)
            {
                mBlinkCost = 0;
                mBlink ^= true;
                mSrc->refresh();
            }
        }
    }
    //------------------------------------------------------------------------
    void EditboxView::setBlinkEnable(bool set)
    {
        mBlinkEnable = set;
    }
    //------------------------------------------------------------------------
    bool EditboxView::isBlinkEnable() const
    {
        return mBlinkEnable;
    }
    //------------------------------------------------------------------------
    void EditboxView::setBlinkTimeout(TimeDurMS ms)
    {
        mBlinkTimeout = ms;
    }
    //------------------------------------------------------------------------
    TimeDurMS EditboxView::getBlinkTimeout() const
    {
        return mBlinkTimeout;
    }
    //------------------------------------------------------------------------
    void EditboxView::setTextLayout(HTextLayout format)
    {
        if (!(format == HTL_Left || format == HTL_Right || format == HTL_Centre))
            N_EXCEPT(InvalidRequest, _I18n("½öÖ§³ÖHTL_Left/HTL_Right/HTL_Centre"));

        mTextLayout = format;
        mSrc->refresh();
    }
    //------------------------------------------------------------------------
    HTextLayout EditboxView::getTextLayout() const
    {
        return mTextLayout;
    }
    //------------------------------------------------------------------------
    bool EditboxView::notifyFont(const Font * obj)
    {
        bool res = WidgetView::notifyFont(obj);

        if (mSrc->getFont() == obj)
        {
            mSrc->refresh();
            return true;
        }
        return res;
    }
    //------------------------------------------------------------------------
}
}