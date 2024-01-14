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
#include "NiiUINumSpinner.h"
#include "NiiUIButton.h"
#include "NiiUIEditbox.h"
#include "NiiUIException.h"
#include "NiiUIWidgetManager.h"
#include "NiiUIWidgetControlItem.h"
#include <iomanip>

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdCurrentValue : public PropertyCmd
    {
    public:
        CmdCurrentValue() : PropertyCmd(N_PropertyUINumSpinner_Current, _T("current_value"),  _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const NumSpinner *>(own)->getCurrent(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<NumSpinner *>(own)->setCurrent(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const NumSpinner *>(own)->getCurrent();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<NumSpinner *>(own)->setCurrent(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCurrentValue();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIId)0.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdStepSize : public PropertyCmd
    {
    public:
        CmdStepSize() : PropertyCmd(N_PropertyUINumSpinner_StepValue, _T("step_size"), _T(""))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const NumSpinner *>(own)->getStepValue(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<NumSpinner *>(own)->setStepValue(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const NumSpinner *>(own)->getStepValue();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<NumSpinner *>(own)->setStepValue(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdStepSize();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIId)1.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdMinimumValue : public PropertyCmd
    {
    public:
        CmdMinimumValue() : PropertyCmd(N_PropertyUINumSpinner_MinValue, _T("minimum_value"),
            _T("Property to get/set the minimum value setting of the spinner.  Value is a NIIf."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const NumSpinner *>(own)->getMinValue(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<NumSpinner *>(own)->setMinValue(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const NumSpinner *>(own)->getMinValue();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<NumSpinner *>(own)->setMinValue(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMinimumValue();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIId)-32768.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdMaximumValue : public PropertyCmd
    {
    public:
        CmdMaximumValue() : PropertyCmd(N_PropertyUINumSpinner_MaxValue, _T("maximum_value"),
            _T("Property to get/set the maximum value setting of the spinner.  Value is a NIIf."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const NumSpinner *>(own)->getMaxValue(), out);
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NIIf temp;
            StrConv::conv(val, temp);
            static_cast<NumSpinner *>(own)->setMaxValue(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NIIf * temp = (NIIf *)out;
            *temp = static_cast<const NumSpinner *>(own)->getMaxValue();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NIIf * temp = (NIIf *)in;
            static_cast<NumSpinner *>(own)->setMaxValue(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaximumValue();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv((NIId)32767.0f, out);
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdTextInputMode : public PropertyCmd
    {
    public:
        CmdTextInputMode() : PropertyCmd(N_PropertyUINumSpinner_NumMode, _T("text_input_mode"),
            _T("Property to get/set the NumMode setting for the spinner.  Value is \"NM_Float\", \"NM_Int\", \"NM_Hex\", or \"NM_Octal\"."))
        {
        }
        void get(const PropertyCmdObj * own, String & out) const
        {
            NumSpinner::NumMode val = static_cast<const NumSpinner *>(own)->getNumMode();
            if(val == NumSpinner::NM_Octal)
            {
                out = _T("NM_Octal");
            }
            else if(val == NumSpinner::NM_Float)
            {
                out = _T("NM_Float");
            }
            else if(val == NumSpinner::NM_Hex)
            {
                out = _T("NM_Hex");
            }
            else if(val == NumSpinner::NM_Int)
            {
                out = _T("NM_Int");
            }
            else
                out = _T("NM_Float");
        }
        void set(PropertyCmdObj * own, const String & val)
        {
            NumSpinner::NumMode temp;
            if(val == _T("NM_Float"))
            {
                temp = NumSpinner::NM_Float;
            }
            else if(val == _T("NM_Hex"))
            {
                temp = NumSpinner::NM_Hex;
            }
            else if(val == _T("NM_Octal"))
            {
                temp = NumSpinner::NM_Octal;
            }
            else
            {
                temp = NumSpinner::NM_Int;
            }
            static_cast<NumSpinner *>(own)->setNumMode(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            NumSpinner::NumMode * temp = (NumSpinner::NumMode *)out;
            *temp = static_cast<const NumSpinner *>(own)->getNumMode();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            NumSpinner::NumMode * temp = (NumSpinner::NumMode *)in;
            static_cast<NumSpinner *>(own)->setNumMode(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTextInputMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("NM_Int");
        }
    };
    //------------------------------------------------------------------------
    N_EVENT(Container, NumSpinner, ValueEvent,                  0);
    N_EVENT(Container, NumSpinner, StepValueEvent,              1);
    N_EVENT(Container, NumSpinner, MaxValueEvent,               2);
    N_EVENT(Container, NumSpinner, MinValueEvent,               3);
    N_EVENT(Container, NumSpinner, NumModeEvent,                4);
    N_EVENT(Container, NumSpinner, EventCount,                  5);
    //------------------------------------------------------------------------
    static const String FloatValidator(_T("-?\\d*\\.?\\d*"));
    static const String IntegerValidator(_T("-?\\d*"));
    static const String HexValidator(_T("[0-9a-fA-F]*"));
    static const String OctalValidator(_T("[0-7]*"));
    //------------------------------------------------------------------------
    // this is taken from stringencoders lib (and modified)
    // http://code.google.com/p/stringencoders/
    //
    // Thanks to FluXy for the heads-up on this one!
    String modp_dtoa(NIId value, int prec);
    //------------------------------------------------------------------------
    NumSpinner::NumSpinner(WidgetID wid, FactoryID fid, Container * own, const String & name,
        LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_Spinner, lid),
        mStepValue(1.0f),
        mCurrentValue(1.0f),
        mMaxValue(32767.0f),
        mMinValue(-32768.0f),
        mNumMode((NumMode)-1),
        mTextAreaView(0),
        mIncreaseView(0),
        mDecreaseView(0),
        mTextArea(0),
        mIncrease(0),
        mDecrease(0)
    {
        notifySignal(NumSpinner::EventCount);
        setupCmd(N_CmdObj_Spinner);
    }
    //------------------------------------------------------------------------
    NumSpinner::~NumSpinner()
    {
        if(mTextArea)
        {
            N_Only(Widget).destroy(mTextArea);
            mTextArea = 0;
        }
        if(mIncrease)
        {
            N_Only(Widget).destroy(mIncrease);
            mIncrease = 0;
        }
        if(mDecrease)
        {
            N_Only(Widget).destroy(mDecrease);
            mDecrease = 0;
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::initChild()
    {
        if(mTextArea == 0)
            mTextArea = static_cast<Editbox *>(N_Only(Widget).create(mTextAreaView, 0, this, N_WIDGET_Editbox));
        if(mIncrease == 0)
            mIncrease = static_cast<PushButton *>(N_Only(Widget).create(mIncreaseView, 0, this, N_WIDGET_PushButton));
        if(mDecrease == 0)
            mDecrease = static_cast<PushButton *>(N_Only(Widget).create(mDecreaseView, 0, this, N_WIDGET_PushButton));

        // setup component controls
        mIncrease->setCursorMultiClick(false);
        mIncrease->setClickAutoEvent(true);
        mDecrease->setCursorMultiClick(false);
        mDecrease->setClickAutoEvent(true);


        // perform event subscriptions.
        mIncrease->bind(Widget::ButtonDownEvent, this, (CommandObjFunctor)&NumSpinner::IncreaseMFunc);
        mDecrease->bind(Widget::ButtonDownEvent, this, (CommandObjFunctor)&NumSpinner::DecreaseMFunc);
        mTextArea->bind(Widget::TextEvent, this, (CommandObjFunctor)&NumSpinner::TextChangeMFunc);

        // final initialisation
        setNumMode(NM_Int);
        setCurrent(0.0f);
        layout();
    }
    //------------------------------------------------------------------------
    void NumSpinner::setCurrent(NIIf value)
    {
        if(value != mCurrentValue)
        {
            // limit input value to within valid range for spinner
            value = N_MAX(NIIf, N_MIN(NIIf, value, mMaxValue), mMinValue);

            mCurrentValue = value;

            WidgetEventArgs args(this);
            onValueChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::setStepValue(NIIf value)
    {
        if(value != mStepValue)
        {
            mStepValue = value;

            WidgetEventArgs args(this);
            onStepValueChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::setMaxValue(NIIf value)
    {
        if(value != mMaxValue)
        {
            mMaxValue = value;

            WidgetEventArgs args(this);
            onMaxValueChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::setMinValue(NIIf value)
    {
        if(value != mMinValue)
        {
            mMinValue = value;

            WidgetEventArgs args(this);
            onMinValueChange(&args);
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::setNumMode(NumMode mode)
    {
        if(mode != mNumMode)
        {
            switch(mode)
            {
            case NM_Float:
                getTextArea()->setRegexString(FloatValidator);
                break;
            case NM_Int:
                getTextArea()->setRegexString(IntegerValidator);
                break;
            case NM_Hex:
                getTextArea()->setRegexString(HexValidator);
                break;
            case NM_Octal:
                getTextArea()->setRegexString(OctalValidator);
                break;
            default:
                N_EXCEPT(InvalidRequest, _I18n("An unknown NumMode was specified."));
            }

            mNumMode = mode;

            WidgetEventArgs args(this);
            onNumModeChange(&args);
        }
    }
    //------------------------------------------------------------------------
    bool NumSpinner::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdCurrentValue());
        dest->add(N_new CmdStepSize());
        dest->add(N_new CmdMinimumValue());
        dest->add(N_new CmdMaximumValue());
        dest->add(N_new CmdTextInputMode());
        return true;
    }
    //------------------------------------------------------------------------
    NIIf NumSpinner::getAreaValue() const
    {
        String tmpTxt(getTextArea()->getText());

        // handle empty and lone '-' or '.' cases
        if(tmpTxt.empty() || (tmpTxt == _T("-")) || (tmpTxt == _T(".")))
        {
            return 0.0f;
        }

        int res, tmp;
        Nui32 utmp;
        NIIf val;

        switch(mNumMode)
        {
        case NM_Float:
            res = Nsscanf(tmpTxt.c_str(), _T("%f"), &val);
            break;
        case NM_Int:
            res = Nsscanf(tmpTxt.c_str(), _T("%d"), &tmp);
            val = static_cast<NIIf>(tmp);
            break;
        case NM_Hex:
            res = Nsscanf(tmpTxt.c_str(), _T("%x"), &utmp);
            val = static_cast<NIIf>(utmp);
            break;
        case NM_Octal:
            res = Nsscanf(tmpTxt.c_str(), _T("%o"), &utmp);
            val = static_cast<NIIf>(utmp);
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("An unknown NumMode was encountered."));
        }

        if(res)
        {
            return val;
        }

        N_EXCEPT(InvalidRequest, _I18n("The string '") + getTextArea()->getText() +
            _I18n("' can not be converted to numerical representation."));
    }
    //------------------------------------------------------------------------
    String NumSpinner::getValueText() const
    {
        StringStream tmp;

        switch(mNumMode)
        {
        case NM_Float:
            return modp_dtoa(mCurrentValue, 9);
            break;
        case NM_Int:
            tmp << static_cast<int>(mCurrentValue);
            break;
        case NM_Hex:
            tmp << std::hex << std::uppercase << static_cast<int>(mCurrentValue);
            break;
        case NM_Octal:
            tmp << std::oct << static_cast<int>(mCurrentValue);
            break;
        default:
            N_EXCEPT(InvalidRequest, _I18n("An unknown NumMode was encountered."));
        }

        return String(tmp.str().c_str());
    }
    //------------------------------------------------------------------------
    void NumSpinner::onFont(const FontEventArgs * arg)
    {
        // Propagate to children
        getTextArea()->setFont(getFont());
    }
    //------------------------------------------------------------------------
    void NumSpinner::onText(const WidgetEventArgs * arg)
    {
        if(mTextArea->getText() != getText())
        {
            // done before doing base class processing so event subscribers see
            // 'updated' version.
            mTextArea->setText(getText());
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::onActivate(const ActivationEventArgs * arg)
    {
        if(!isFinalActive())
        {
            Widget::onActivate(arg);
            if(!mTextArea->isFinalActive())
            {
                mTextArea->activate();
            }
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::onValueChange(const WidgetEventArgs * arg)
    {
        // mute to save doing unnecessary events work.
        bool enable = mTextArea->isSignalEnable();
        mTextArea->setSignalEnable(enable);

        // Update text with new value.
        // (allow empty and '-' cases to equal 0 with no text change required)
        if(!(mCurrentValue == 0 && (mTextArea->getText().empty() || mTextArea->getText() == _T("-"))))
        {
            mTextArea->setText(getValueText());
        }
        // restore previous mute state.
        mTextArea->setSignalEnable(enable);

        signal(ValueEvent, arg);
    }
    //------------------------------------------------------------------------
    void NumSpinner::onStepValueChange(const WidgetEventArgs * arg)
    {
        signal(StepValueEvent, arg);
    }
    //------------------------------------------------------------------------
    void NumSpinner::onMaxValueChange(const WidgetEventArgs * arg)
    {
        signal(MaxValueEvent, arg);

        if(mCurrentValue > mMaxValue)
        {
            setCurrent(mMaxValue);
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::onMinValueChange(const WidgetEventArgs * arg)
    {
        signal(MinValueEvent, arg);

        if(mCurrentValue < mMinValue)
        {
            setCurrent(mMinValue);
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::onNumModeChange(const WidgetEventArgs * arg)
    {
        // update edit box text to reflect new mode.
        // mute to save doing unnecessary events work.
        bool enable = mTextArea->isSignalEnable();
        mTextArea->setSignalEnable(false);
        // Update text with new value.
        mTextArea->setText(getValueText());
        // restore previous mute state.
        mTextArea->setSignalEnable(enable);

        signal(NumModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void NumSpinner::IncreaseMFunc(const EventArgs * e)
    {
        if(static_cast<const CursorEventArgs *>(e)->mMouseArg.mButton == MB_Left)
        {
            setCurrent(mCurrentValue + mStepValue);
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::DecreaseMFunc(const EventArgs * e)
    {
        if(static_cast<const CursorEventArgs *>(e)->mMouseArg.mButton == MB_Left)
        {
            setCurrent(mCurrentValue - mStepValue);
        }
    }
    //------------------------------------------------------------------------
    void NumSpinner::TextChangeMFunc(const EventArgs *)
    {
        // set this windows text to match
        setText(getTextArea()->getText());
        // update value
        setCurrent(getAreaValue());
    }
    //------------------------------------------------------------------------
    //////////////////////////////////////////////////////////////////////////
    // The following are slightly modified function(s) taken from the
    // stringencoders library.
    // http://code.google.com/p/stringencoders/
    //
    // Thanks to FluXy for the heads-up on this one!
    /*
     * Copyright 2005, 2006, 2007
     * Nick Galbreath -- nickg [at] modp [dot] com
     * All rights reserved.
     *
     * Redistribution and use in source and binary forms, with or without
     * modification, are permitted provided that the following conditions are
     * met:
     *
     *   Redistributions of source code must retain the above copyright
     *   notice, this list of conditions and the following disclaimer.
     *
     *   Redistributions in binary form must reproduce the above copyright
     *   notice, this list of conditions and the following disclaimer in the
     *   documentation and/or other materials provided with the distribution.
     *
     *   Neither the name of the modp.com nor the names of its
     *   contributors may be used to endorse or promote products derived from
     *   this software without specific prior written permission.
     *
     * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
     * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
     * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
     * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
     * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
     * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
     * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
     * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
     * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
     * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
     * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
     *
     * This is the standard "new" BSD license:
     * http://www.opensource.org/licenses/bsd-license.php
     */

    /**
     * Powers of 10
     * 10^0 to 10^9
     */
    static const NIId pow10[] =
    {
        1, 10, 100, 1000,
        10000, 100000, 1000000,
        10000000, 100000000, 1000000000
    };

    void strreverse(Ntchar * begin, Ntchar * end)
    {
        Ntchar aux;
        while (end > begin)
            aux = *end, *end-- = *begin, *begin++ = aux;
    }

    String modp_dtoa(NIId value, int prec)
    {
        /* if input is larger than thres_max, revert to exponential */
        const NIId thres_max = (NIId)(0x7FFFFFFF);

        NIId diff = 0.0;
        Ntchar str[64];
        Ntchar * wstr = str;

        if (prec < 0)
        {
            prec = 0;
        }
        else if (prec > 9)
        {
            /* precision of >= 10 can lead to overflow errors */
            prec = 9;
        }

        /* we'll work in positive values and deal with the
           negative sign issue later */
        int neg = 0;
        if(value < 0)
        {
            neg = 1;
            value = -value;
        }

        int whole = (int) value;
        NIId tmp = (value - whole) * pow10[prec];
        Nui32 frac = (Nui32)(tmp);
        diff = tmp - frac;

        if (diff > 0.5)
        {
            ++frac;
            /* handle rollover, e.g.  case 0.99 with prec 1 is 1.0  */
            if (frac >= pow10[prec])
            {
                frac = 0;
                ++whole;
            }
        }
        else if (diff == 0.5 && ((frac == 0) || (frac & 1)))
        {
            /* if halfway, round up if odd, OR
               if last digit is 0.  That last part is strange */
            ++frac;
        }

        /* for very large numbers switch back to native sprintf for exponentials.
           anyone want to write code to replace this? */
        /*
           normal printf behavior is to print EVERY whole number digit
           which can be 100s of characters overflowing your buffers == bad
        */
        if (value > thres_max)
        {
            Nsntprintf(str, sizeof(str), _T("%e"), neg ? -value : value);
            return String(str);
        }

        if (prec == 0)
        {
            diff = value - whole;
            if (diff > 0.5)
            {
                /* greater than 0.5, round up, e.g. 1.6 -> 2 */
                ++whole;
            } else if (diff == 0.5 && (whole & 1)) {
                /* exactly 0.5 and ODD, then round up */
                /* 1.5 -> 2, but 2.5 -> 2 */
                ++whole;
            }
        }
        else
        {
            // This section has been modifed by the CEGUI project to eliminate
            // the output of trailing zeros in the fractional part.
            bool non_zero = false;
            int count = prec;
            Nui32 digit = 0;
            // now do fractional part, as an unsigned number
            do
            {
                --count;
                digit = (frac % 10);
                if (non_zero || (digit != 0))
                {
                    *wstr++ = 48 + digit;
                    non_zero = true;
                }
            } while (frac /= 10);
            // add extra 0s
            if (non_zero) {
                while (count-- > 0) *wstr++ = '0';
                // add decimal
                *wstr++ = '.';
            }
        }

        // do whole part
        // Take care of sign
        // Conversion. Number is reversed.
        do 
            *wstr++ = 48 + (whole % 10); 
        while (whole /= 10);
        if (neg) 
        {
            *wstr++ = '-';
        }
        *wstr='\0';
        strreverse(str, wstr-1);

        return String(str);
    }
    // The above are slightly modified function(s) taken from the
    // stringencoders library.
    // http://code.google.com/p/stringencoders/
    //------------------------------------------------------------------------
}
}
