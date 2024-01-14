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
#include "NiiUICombobox.h"
#include "NiiUIButton.h"
#include "NiiUIListbox.h"
#include "NiiUIWidgetManager.h"

namespace NII
{
namespace UI
{
    //------------------------------------------------------------------------
    class _EngineInner CmdReadOnly : public PropertyCmd
    {
    public:
        CmdReadOnly() : PropertyCmd(N_PropertyUICombobox_ReadOnly, _T("read_only"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->isReadOnly(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setReadOnly(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Combobox *>(own)->isReadOnly();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Combobox *>(own)->setReadOnly(*temp);
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
    class _EngineInner CmdValidationString : public PropertyCmd
    {
    public:
        CmdValidationString() : PropertyCmd(N_PropertyUICombobox_ValidationString, _T("validation_string"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            out = static_cast<const Combobox *>(own)->getRegexString();
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            static_cast<Combobox *>(own)->setRegexString(val);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            String * temp = (String *)out;
            *temp = static_cast<const Combobox *>(own)->getRegexString();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            String * temp = (String *)in;
            static_cast<Combobox *>(own)->setRegexString(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdValidationString();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T(".*");
        }
    };

    class _EngineInner CmdCaretIndex : public PropertyCmd
    {
    public:
        CmdCaretIndex() : PropertyCmd(N_PropertyUICombobox_CaretIndex, _T("caret_index"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->getInput(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setInput(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Combobox *>(own)->getInput();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Combobox *>(own)->setInput(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdCaretIndex();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };

    class _EngineInner CmdSelectionStart : public PropertyCmd
    {
    public:
        CmdSelectionStart() : PropertyCmd(N_PropertyUICombobox_SelectionStartIndex, _T("selection_start"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->getSelectBegin(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setSelectBegin(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Combobox *>(own)->getSelectBegin();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Combobox *>(own)->setSelectBegin(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectionStart();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };

    class _EngineInner CmdSelectionLength : public PropertyCmd
    {
    public:
        CmdSelectionLength() : PropertyCmd(N_PropertyUICombobox_SelectionLength, _T("selection_length"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->getSelectLength(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setSelectLength(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Combobox *>(own)->getSelectLength();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Combobox *>(own)->setSelectLength(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSelectionLength();
        }

        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            out = _T("0");
        }
    };

    class _EngineInner CmdMaxTextLength : public PropertyCmd
    {
    public:
        CmdMaxTextLength() : PropertyCmd(N_PropertyUICombobox_MaxTextLength, _T("max_text_length"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->getMaxLength(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            Nui32 temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setMaxLength(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            Nui32 * temp = (Nui32 *)out;
            *temp = static_cast<const Combobox *>(own)->getMaxLength();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            Nui32 * temp = (Nui32 *)in;
            static_cast<Combobox *>(own)->setMaxLength(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdMaxTextLength();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(out.max_size(), out);
        }
    };
    class _EngineInner CmdSortList : public PropertyCmd
    {
    public:
        CmdSortList() : PropertyCmd(N_PropertyUICombobox_SortEnable, _T("sort_list"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->isSortEnable(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setSortEnable(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Combobox *>(own)->isSortEnable();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Combobox *>(own)->setSortEnable(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSortList();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdForceVertScrollbar : public PropertyCmd
    {
    public:
        CmdForceVertScrollbar() : PropertyCmd(N_PropertyUICombobox_VScrollAlwaysShow, _T("force_vertscrollbar"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->isForceVScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->SetForceVScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Combobox *>(own)->isForceVScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Combobox *>(own)->SetForceVScrollbar(*temp);
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

    class _EngineInner CmdForceHorzScrollbar : public PropertyCmd
    {
    public:
        CmdForceHorzScrollbar() : PropertyCmd(N_PropertyUICombobox_HScrollAlwaysShow, _T("force_horzscrollbar"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->isForceHScrollbar(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->SetForceHScrollbar(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Combobox *>(own)->isForceHScrollbar();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Combobox *>(own)->SetForceHScrollbar(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdForceHorzScrollbar();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdSingleClickMode : public PropertyCmd
    {
    public:
        CmdSingleClickMode() : PropertyCmd(N_PropertyUICombobox_SingleClickEnable, _T("single_click_mode"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->isAutoDropDown(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setAutoDropDown(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Combobox *>(own)->isAutoDropDown();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Combobox *>(own)->setAutoDropDown(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdSingleClickMode();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdAutoSizeListHeight : public PropertyCmd
    {
    public:
        CmdAutoSizeListHeight() : PropertyCmd(N_PropertyUICombobox_AutoSizeListHeight, _T("auto_size_list_height"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->isListboxAutoHeight(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setListboxAutoHeight(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Combobox *>(own)->isListboxAutoHeight();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Combobox *>(own)->setListboxAutoHeight(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoSizeListHeight();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };

    class _EngineInner CmdAutoSizeListWidth : public PropertyCmd
    {
    public:
        CmdAutoSizeListWidth() : PropertyCmd(N_PropertyUICombobox_AutoSizeListWidth, _T("auto_size_list_width"), _T(""))
        {
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, String & out) const
        {
            StrConv::conv(static_cast<const Combobox *>(own)->isListboxAutoWidth(), out);
        }
        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, const String & val)
        {
            bool temp;
            StrConv::conv(val, temp);
            static_cast<Combobox *>(own)->setListboxAutoWidth(temp);
        }
        /// @copydetails PropertyCmd::get
        void get(const PropertyCmdObj * own, void * out)
        {
            bool * temp = (bool *)out;
            *temp = static_cast<const Combobox *>(own)->isListboxAutoWidth();
        }

        /// @copydetails PropertyCmd::set
        void set(PropertyCmdObj * own, void * in)
        {
            bool * temp = (bool *)in;
            static_cast<Combobox *>(own)->setListboxAutoWidth(*temp);
        }

        /// @copydetails PropertyCmd::clone
        PropertyCmd * clone() const
        {
            return N_new CmdAutoSizeListWidth();
        }
        /// @copydetails PropertyCmd::getDefault
        void getDefault(String & out) const
        {
            StrConv::conv(false, out);
        }
    };
    //------------------------------------------------------------------------
    // event names from edit box
    N_EVENT(Container, Combobox, ReadOnlyModeEvent,                 0);
    N_EVENT(Container, Combobox, RegexStringEvent,                  1);
    N_EVENT(Container, Combobox, MaxLengthEvent,                    2);
    N_EVENT(Container, Combobox, TextValidChangeEvent,              3);
    N_EVENT(Container, Combobox, InputPosEvent,                     4);
    N_EVENT(Container, Combobox, SelectEvent,                       5);
    N_EVENT(Container, Combobox, InputFullEvent,                    6);
    N_EVENT(Container, Combobox, InputAcceptEvent,                  7);
    // event names from list box
    N_EVENT(Container, Combobox, ItemListEvent,                     8);
    N_EVENT(Container, Combobox, ItemSelectEvent,                   9);
    N_EVENT(Container, Combobox, SortModeEvent,                     10);
    N_EVENT(Container, Combobox, VScrollModeEvent,                  11);
    N_EVENT(Container, Combobox, HScrollModeEvent,                  12);
    // events we produce / generate ourselves
    N_EVENT(Container, Combobox, ListShowEvent,                     13);
    N_EVENT(Container, Combobox, ListRemoveEvent,                   14);
    N_EVENT(Container, Combobox, SelectAcceptEvent,                 15);
    N_EVENT(Container, Combobox, EventCount,                        16);
    //------------------------------------------------------------------------
    #define EditboxName _T("__auto_editbox__")
    #define DropListName _T("__auto_droplist__")
    #define ButtonName _T("__auto_button__")
    //------------------------------------------------------------------------
    Combobox::Combobox(WidgetID wid, FactoryID fid, Container * own, const String & name, LangID lid) :
        Container(wid, fid, own, name, N_CmdObj_Combobox, lid),
        mAutoDropDown(false),
        mAutoHeight(false),
        mAutoWidth(false)
    {
        notifySignal(Combobox::EventCount);
        setupCmd(N_CmdObj_Combobox);
    }
    //------------------------------------------------------------------------
    Combobox::~Combobox(void)
    {
    }
    //------------------------------------------------------------------------
    void Combobox::initChild()
    {
        Editbox * editbox = getEditbox();
        ComboDropList * droplist = getListBox();
        PushButton * button = getListButton();
        droplist->setFont(mFont);
        editbox->setFont(mFont);

        // ban properties forwarded from here
        droplist->setWrite(N_PropertyUIWidget_Visible, false);
        editbox->setWrite(N_PropertyUIEditbox_MaxLength, false);

        // internal event wiring
        button->bind(PushButton::ButtonDownEvent, this,         (CommandObjFunctor)&Combobox::listButtonPressPrc);
        droplist->bind(ComboDropList::SelectAcceptEvent, this,  (CommandObjFunctor)&Combobox::listAcceptPrc);
        droplist->bind(Widget::HideEvent, this,                 (CommandObjFunctor)&Combobox::listHidePrc);
        editbox->bind(Widget::ButtonDownEvent, this,            (CommandObjFunctor)&Combobox::editboxPressPrc);
        // event forwarding setup
        editbox->bind(Editbox::ReadOnlyModeEvent, this,         &Combobox::onReadOnlyMode);
        editbox->bind(Editbox::RegexStringEvent, this,          &Combobox::onRegexString);
        editbox->bind(Editbox::MaxLengthEvent, this,        &Combobox::onMaxLength);
        editbox->bind(Editbox::TextValidChangeEvent, this,      &Combobox::onTextValid);
        editbox->bind(Editbox::InputPosEvent, this,             &Combobox::onInputPos);
        editbox->bind(Editbox::SelectChangeEvent, this,         &Combobox::onTextSelect);
        editbox->bind(Editbox::InputFullEvent, this,            &Combobox::onTextFull);
        editbox->bind(Editbox::InputAcceptEvent, this,          &Combobox::onTextAccept);
        editbox->bind(Editbox::TextEvent, this,					(CommandObjFunctor)&Combobox::editboxTextPrc);
        droplist->bind(Listbox::ItemListEvent, this,            &Combobox::onItemList);
        droplist->bind(Listbox::SelectEvent, this,				&Combobox::onListSelect);
        droplist->bind(Listbox::SortModeEvent, this,            &Combobox::onSortMode);
        droplist->bind(Listbox::VScrollModeEvent, this,         &Combobox::onVScrollbarMode);
        droplist->bind(Listbox::HScrollModeEvent, this,         &Combobox::onHScrollbarMode);

        // put components in their initial positions
        layout();
    }
    //------------------------------------------------------------------------
    void Combobox::showDropList()
    {
        getListBox()->fit(mAutoWidth, mAutoHeight);

        // Display the box
        ComboDropList * droplist = getListBox();
        droplist->show();
        droplist->activate();
        droplist->focus();

        // Fire off event
        WidgetEventArgs args(this);
        onListShow(&args);
    }
    //------------------------------------------------------------------------
    void Combobox::hideDropList()
    {
        getListBox()->lost();
    }
    //------------------------------------------------------------------------
    void Combobox::setItemSelect(ListboxItem * item, bool state)
    {
        bool was_selected = (item && item->isSelect());
        getListBox()->setItemSelect(item, state);
        updateItemState(item, state, was_selected);
    }
    //------------------------------------------------------------------------
    void Combobox::setItemSelect(NCount item_index, bool state)
    {
        ComboDropList * droplist = getListBox();

        ListboxItem * item = (droplist->getItemCount() > item_index) ?
            droplist->getItem(item_index) : 0;

        bool was_selected = (item && item->isSelect());
        droplist->setItemSelect(item_index, state);
        updateItemState(item, state, was_selected);
    }
    //------------------------------------------------------------------------
    void Combobox::onReadOnlyMode(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(ReadOnlyModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onRegexString(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(RegexStringEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onMaxLength(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(MaxLengthEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onTextValid(const RegexMatchStateEventArgs * arg)
    {
        arg->mWidget = this;
        signal(TextValidChangeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onInputPos(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(InputPosEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onTextSelect(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(SelectEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onTextFull(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(InputFullEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onTextAccept(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        ComboDropList * droplist = getListBox();
        if(ListboxItem * item = droplist->getItem(getEditbox()->getText(), 0))
        {
            droplist->setItemSelect(item, true);
            droplist->visibleItem(item);
        }
        else
        {
            droplist->unselectAll();
        }
        signal(InputAcceptEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onItemList(const WidgetEventArgs * arg)
    {
        if(isListBoxVisible())
            getListBox()->fit(mAutoWidth, mAutoHeight);
        arg->mWidget = this;
        signal(ItemListEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onListSelect(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(ItemSelectEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onSortMode(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(SortModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onVScrollbarMode(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(VScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onHScrollbarMode(const WidgetEventArgs * arg)
    {
        arg->mWidget = this;
        signal(HScrollModeEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onListShow(const WidgetEventArgs * arg)
    {
        mSheet->resetCursorOver();
        getListButton()->press(true);
        signal(ListShowEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onListRemove(const WidgetEventArgs * arg)
    {
        mSheet->resetCursorOver();
        getListButton()->press(false);
        signal(ListRemoveEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onSelectAccept(const WidgetEventArgs * arg)
    {
        signal(SelectAcceptEvent, arg);
    }
    //------------------------------------------------------------------------
    void Combobox::onFont(const FontEventArgs * e)
    {
        // Propagate to children
        getEditbox()->setFont(mFont);
        getListBox()->setFont(mFont);

        Widget::onFont(e);
    }
    //------------------------------------------------------------------------
    void Combobox::onText(const WidgetEventArgs * e)
    {
        Editbox * editbox = getEditbox();
        // update ourselves only if needed (prevents perpetual event loop & stack overflow)
        if(editbox->getText() != getText())
        {
            // done before doing base class processing so event subscribers see 'updated' version of this.
            editbox->setText(getText());
            ComboDropList * droplist = getListBox();
            if(ListboxItem * item = droplist->getItem(getEditbox()->getText(), 0))
            {
                droplist->setItemSelect(item, true);
                droplist->visibleItem(item);
            }
            else
            {
                droplist->unselectAll();
            }
            Widget::onText(e);
        }
    }
    //------------------------------------------------------------------------
    void Combobox::listButtonPressPrc(const EventArgs *)
    {
        ComboDropList * droplist = getListBox();
        if(ListboxItem * item = droplist->getItem(getEditbox()->getText(), 0))
        {
            droplist->setItemSelect(item, true);
            droplist->visibleItem(item);
        }
        else
        {
            droplist->unselectAll();
        }
        showDropList();
    }
    //------------------------------------------------------------------------
    void Combobox::setListboxAutoHeight(bool auto_size)
    {
        mAutoHeight = auto_size;
        if(mAutoHeight && isListBoxVisible())
            getListBox()->fit(mAutoWidth, mAutoHeight);
    }
    //------------------------------------------------------------------------
    void Combobox::setListboxAutoWidth(bool auto_size)
    {
        mAutoWidth = auto_size;
        if(mAutoWidth && isListBoxVisible())
            getListBox()->fit(mAutoWidth, mAutoHeight);
    }
    //------------------------------------------------------------------------
    void Combobox::listAcceptPrc(const EventArgs * e)
    {
        // copy the text from the selected item into the edit box
        ListboxItem * item = static_cast<ComboDropList*>(static_cast<const WidgetEventArgs*>(e)->mWidget)->getFirstSelect();

        if(item)
        {
            Editbox * editbox = getEditbox();
            // Put the text from the list item into the edit box
            editbox->setText(item->getText());

            // select text if it's editable, and move caret to end
            if(!isReadOnly())
            {
                editbox->setSelect(0, item->getText().length());
                editbox->setInput(item->getText().length());
            }

            editbox->setInput(0);
            editbox->activate();

            // fire off an event of our own
            WidgetEventArgs args(this);
            onSelectAccept(&args);
        }
    }
    //------------------------------------------------------------------------
    void Combobox::listHidePrc(const EventArgs *)
    {
        WidgetEventArgs args(this);
        onListRemove(&args);
    }
    //------------------------------------------------------------------------
    void Combobox::editboxPressPrc(const EventArgs * e)
    {
        // only interested in left button
        if(static_cast<const MouseArgs *>(e)->mButton == MB_Left)
        {
            Editbox * editbox = getEditbox();

            // if edit box is read-only, show list
            if(editbox->isReadOnly())
            {
                ComboDropList * droplist = getListBox();

                // if there is an item with the same text as the edit box, pre-select it
                ListboxItem * item = droplist->getItem(editbox->getText(), 0);

                if(item)
                {
                    droplist->setItemSelect(item, true);
                    droplist->visibleItem(item);
                }
                // no matching item, so select nothing
                else
                {
                    droplist->unselectAll();
                }

                showDropList();
            }
        }
    }
    //------------------------------------------------------------------------
    bool Combobox::initCmd(PropertyCmdSet * dest)
    {
        dest->add(N_new CmdReadOnly());
        dest->add(N_new CmdValidationString());
        dest->add(N_new CmdCaretIndex());
        dest->add(N_new CmdSelectionStart());
        dest->add(N_new CmdSelectionLength());
        dest->add(N_new CmdMaxTextLength());
        dest->add(N_new CmdSortList());
        dest->add(N_new CmdForceVertScrollbar());
        dest->add(N_new CmdForceHorzScrollbar());
        dest->add(N_new CmdSingleClickMode());
        dest->add(N_new CmdAutoSizeListHeight());
        dest->add(N_new CmdAutoSizeListWidth());
        return true;
    }
    //------------------------------------------------------------------------
    void Combobox::activateEditbox()
    {
        Editbox * editbox = getEditbox();
        if(!editbox->isFinalActive())
        {
            editbox->activate();
        }
    }
    //------------------------------------------------------------------------
    void Combobox::deactivateEditbox()
    {
        Editbox * editbox = getEditbox();
        if(!editbox->isFinalActive())
        {
            editbox->deactivate();
        }
    }
    //------------------------------------------------------------------------
    void Combobox::onActivate(const ActivationEventArgs * e)
    {
        if(!isFinalActive())
        {
            Widget::onActivate(e);
            activateEditbox();
        }
    }
    //------------------------------------------------------------------------
    void Combobox::onSize(const WidgetEventArgs * e)
    {
        if (isListBoxVisible())
            getListBox()->fit(mAutoWidth, mAutoHeight);

        Container::onSize(e);
    }
    //------------------------------------------------------------------------
    void Combobox::setAutoDropDown(bool set)
    {
        mAutoDropDown = set;
        getListBox()->setAutoDropDown(set);
    }
    //------------------------------------------------------------------------
    Editbox * Combobox::getEditbox() const
    {
        return static_cast<Editbox *>(get(EditboxName));
    }
    //------------------------------------------------------------------------
    PushButton * Combobox::getListButton() const
    {
        return static_cast<PushButton *>(get(ButtonName));
    }
    //------------------------------------------------------------------------
    ComboDropList * Combobox::getListBox() const
    {
        return static_cast<ComboDropList *>(get(DropListName));
    }
    //------------------------------------------------------------------------
    void Combobox::updateItemState(const ListboxItem * item, bool new_state, bool old_state)
    {
        if(!new_state)
        {
            if(getText() == item->getText())
                setText(_T(""));
        }
        else
        {
            if(!old_state)
                setText(item->getText());
        }
    }
    //------------------------------------------------------------------------
    bool Combobox::intersects(const Vector2f &, const bool) const
    {
        return false;
    }
    //------------------------------------------------------------------------
    void Combobox::editboxTextPrc(const EventArgs * e)
    {
        // set this windows text to match
        setText(((const WidgetEventArgs&)e).mWidget->getText());
    }
    //------------------------------------------------------------------------
}
}