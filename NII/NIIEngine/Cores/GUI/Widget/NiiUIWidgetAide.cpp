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
#include "NiiUIWidgetAide.h"
#include "NiiUISheet.h"
#include "NiiPixelManager.h"
#include "NiiUIWidgetManager.h"
#include "NiiUITooltip.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // CursorWidgetAide
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdMouseCursorImage : public PropertyCmd
    {
    public:
        CmdMouseCursorImage() : PropertyCmd(N_PropertyUICursorWidgetAide_MouseCursor, _T("mouse_cursor_image"),
            _T("Property to get/set the mouse cursor image for the Widget.  Value should be \"<image name>\"."))
        {}
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            PixelBuffer * val = static_cast<const CursorWidgetAide *>(own)->getMouseCursor();
            out = val ? N_conv(val->getGroupID()) + _T("|") +N_conv(val->getID()) : _T("0|0");
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            StringList temp;
            StrUtil::split(val, temp, _T("|"));
            Nui32 rid, pid;
            StrConv::conv(temp[0], rid);
            StrConv::conv(temp[1], pid);
            static_cast<CursorWidgetAide *>(own)->setMouseCursor(N_Only(Pixel).get(rid, pid));
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui64 * temp = (Nui64 *)out;
            PixelBuffer * pb = static_cast<const CursorWidgetAide *>(own)->getMouseCursor();
            *temp = pb ? ((Nui64)pb->getGroupID() << 32) | pb->getID() : 0;
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui64 * temp = (Nui64 *)in;
            Nui32 rid, pid;
            rid = *temp >> 32;
            pid = *temp & 0xffffffff;
            static_cast<CursorWidgetAide *>(own)->setMouseCursor(N_Only(Pixel).get(rid, pid));
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMouseCursorImage();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0|0");
        }
    };
    //------------------------------------------------------------------------
    CursorWidgetAide::CursorWidgetAide(Widget * dst, LangID lid) :
        WidgetAide(dst, N_CmdObj_UICursorWidgetAide, lid),
        mImage(0)
    {
    }
    //------------------------------------------------------------------------
    CursorWidgetAide::~CursorWidgetAide()
    {
    }
    //------------------------------------------------------------------------
    void CursorWidgetAide::setMouseCursor(PixelBuffer * pb)
    {
        mImage = pb;

        if(mSheet->getCursorOver() == mCurrent)
            mSheet->getCursor()->setImage(pb);
    }
    //------------------------------------------------------------------------
    PixelBuffer * CursorWidgetAide::getMouseCursor(bool _default) const
    {
        if(mImage)
            return mImage;
        else
            return _default ? mSheet->getCursor()->getDefaultImage() : 0;
    }
    //------------------------------------------------------------------------
    bool CursorWidgetAide::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdMouseCursorImage());
        return true;
    }
    //------------------------------------------------------------------------
    void CursorWidgetAide::onCursorEnter(const CursorEventArgs * arg)
    {
        mSheet->getCursor()->setImage(getMouseCursor());
    }
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // TooltipWidgetAide
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    class _EngineInner CmdTooltipType : public PropertyCmd
    {
    public:
        CmdTooltipType() : PropertyCmd(N_PropertyUITooltipWidgetAide_TooltipType, _T("tooltip_type"),
            _T("Property to get/set the custom tooltip for the window. Value is the type name of the custom tooltip. If \"\", the default UIManager tooltip is used."))
        {}
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TooltipWidgetAide *>(own)->getTooltipType(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 sid;
            StrConv::conv(val, sid);
            static_cast<TooltipWidgetAide *>(own)->setTooltipType(sid);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const TooltipWidgetAide *>(own)->getTooltipType();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<TooltipWidgetAide *>(own)->setTooltipType(*temp);
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTooltipType();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdTooltipText : public PropertyCmd
    {
    public:
        CmdTooltipText() : PropertyCmd(N_PropertyUITooltipWidgetAide_TooltipText, _T("tooltip_text"),
            _T("Property to get/set the tooltip text for the window. Value is the tooltip text for the window."))
        {}
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const TooltipWidgetAide *>(own)->getTooltipText();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<TooltipWidgetAide *>(own)->setTooltipText(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const TooltipWidgetAide *>(own)->getTooltipText();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            String * temp = (String *)in;
            static_cast<TooltipWidgetAide *>(own)->setTooltipText(*temp);
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdTooltipText();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("");
        }
    };
    //------------------------------------------------------------------------
    class _EngineInner CmdInheritsTooltipText : public PropertyCmd
    {
    public:
        CmdInheritsTooltipText() : PropertyCmd(N_PropertyUITooltipWidgetAide_ParentTooltipText, _T("innherits_tooltip_text"),
            _T("Property to get/set whether the window inherits its parents tooltip text when it has none of its own. Value is either \"True\" or \"False\"."))
        {}
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const TooltipWidgetAide *>(own)->isParentTooltipText(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<TooltipWidgetAide *>(own)->setParentTipsText(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const TooltipWidgetAide *>(own)->isParentTooltipText();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<TooltipWidgetAide *>(own)->setParentTipsText(*temp);
        }
        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdInheritsTooltipText();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(true, out);
        }
    };
    //------------------------------------------------------------------------
    TooltipWidgetAide::TooltipWidgetAide(Widget * dst, LangID lid) :
        WidgetAide(dst, N_CmdObj_UITooltipWidgetAide,lid),
        mCustomTip(0),
        mParentTipsText(true),
        mGenTip(false)
    {
        setupCmd(N_CmdObj_UITooltipWidgetAide);
    }
    //------------------------------------------------------------------------
    TooltipWidgetAide::~TooltipWidgetAide()
    {
    }
    //------------------------------------------------------------------------
    void TooltipWidgetAide::setTooltipType(StyleID type)
    {
        // destroy current custom tooltip if one exists and we created it
        if(mCustomTip && mGenTip)
            N_Only(Widget).destroy(mCustomTip);

        if(type == 0)
        {
            mCustomTip = 0;
            mGenTip = false;
        }
        else
        {
            try
            {
                mCustomTip = static_cast<Tooltip *>(N_Only(Widget).create(type, 0, 0));
                mCustomTip->setAutoWindow(true);
                mGenTip = true;
                mType = type;
            }
            catch(InvalidParamException &)
            {
                mCustomTip = 0;
                mGenTip = false;
                mType = 0;
            }
        }
    }
    //------------------------------------------------------------------------
    StyleID TooltipWidgetAide::getTooltipType() const
    {
        return mCustomTip == 0 ? mType : mCustomTip->getType();
    }
    //------------------------------------------------------------------------
    void TooltipWidgetAide::setTooltip(Tooltip * obj)
    {
        if(mCustomTip && mGenTip)
            N_Only(Widget).destroy(mCustomTip);

        mGenTip = false;
        mCustomTip = obj;
    }
    //------------------------------------------------------------------------
    Tooltip * TooltipWidgetAide::getTooltip() const
    {
        Tooltip * re;
        if(mCustomTip == 0)
        {
            re = mSheet->getTooltip();
            mType = 0;
        }
        else
        {
            re = mCustomTip;
            mType = mCustomTip->getType();
        }
        return re;
    }
    //------------------------------------------------------------------------
    bool TooltipWidgetAide::isDefault() const
    {
        return mCustomTip == 0;
    }
    //------------------------------------------------------------------------
    void TooltipWidgetAide::setTooltipText(const String & tip)
    {
        mTipsText = tip;

        Tooltip * tooltip = getTooltip();

        if(tooltip && tooltip->getDest() == mCurrent)
            tooltip->setText(tip);
    }
    //------------------------------------------------------------------------
    const String & TooltipWidgetAide::getTooltipText() const
    {
        if(mParentTipsText && mCurrent && mTipsText.empty())
            return mCurrent->getTooltipText();
        else
            return mTipsText;
    }
    //------------------------------------------------------------------------
    bool TooltipWidgetAide::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdTooltipType());
        dest->add(N_new CmdTooltipText());
        dest->add(N_new CmdInheritsTooltipText());
        return true;
    }
    //------------------------------------------------------------------------
    void TooltipWidgetAide::onCursorEnter(const CursorEventArgs * arg)
    {
        Tooltip * tip = getTooltip();
        if(tip && !mCurrent->isAncestor(tip))
            tip->setDest(mCurrent);
    }
    //------------------------------------------------------------------------
    void TooltipWidgetAide::onCursorLeave(const CursorEventArgs * arg)
    {
        Widget *  mw = mSheet->getCursorOver();
        Tooltip * tip = getTooltip();
        if(tip && mw != tip && !(mw && mw->isAncestor(tip)))
            tip->setDest(0);
    }
    //------------------------------------------------------------------------
    void TooltipWidgetAide::onCursorMove(const CursorEventArgs * arg)
    {
        Tooltip * tip = getTooltip();
        if (tip)
        {
            tip->resetState();
        }
    }
    //------------------------------------------------------------------------
    void TooltipWidgetAide::onButtonDown(const CursorEventArgs * arg)
    {
        Tooltip * tip = getTooltip();
        if (tip)
        {
            tip->setDest(0);
        }
    }
    //------------------------------------------------------------------------
}
}