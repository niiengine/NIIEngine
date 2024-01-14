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
#ifndef _NII_COMBOBOX_H_
#define _NII_COMBOBOX_H_

#include "NiiUIPreInclude.h"
#include "NiiUIEditbox.h"
#include "NiiUIListbox.h"
#include "NiiRegexManager.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Combobox : public Container
    {
    public:
        typedef RegexMatcher::MatchState MatchState;
    public:
        Combobox(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~Combobox();

        /** 
        @version NIIEngine 3.0.0
        */
        void setAutoDropDown(bool b);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isAutoDropDown() const { return mAutoDropDown; }

        /**
        @version NIIEngine 3.0.0
        */
        Editbox * getEditbox() const;

        /**
        @version NIIEngine 3.0.0
        */
        PushButton * getListButton() const;

        /**
        @version NIIEngine 3.0.0
        */
        ComboDropList * getListBox() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setListboxAutoHeight(bool set);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isListboxAutoHeight() const { return mAutoHeight; }
        
        /**
        @version NIIEngine 3.0.0
        */
        void setListboxAutoWidth(bool set);

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isListboxAutoWidth() const{ return mAutoWidth;}

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isListBoxVisible() const{ return getListBox()->isFinalVisible(); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void SetForceVScrollbar(bool set){ getListBox()->SetForceVScrollbar(set); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isForceVScrollbar() const{ return getListBox()->isForceVScrollbar();}

        /**
        @version NIIEngine 3.0.0
        */
        inline void SetForceHScrollbar(bool set){ getListBox()->SetForceHScrollbar(set); }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isForceHScrollbar() const{ return getListBox()->isForceHScrollbar();}
        
        /**
        @version NIIEngine 3.0.0
        */
        inline bool isFinalActive() const { return getEditbox()->isFinalActive(); }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isReadOnly() const{ return getEditbox()->isReadOnly(); }

        /** 获取当前有效的字符串
        @version NIIEngine 3.0.0
        */
        inline const String & getRegexString() const { return getEditbox()->getRegexString(); }

        /** 获取当前插入字符的位置
        @version NIIEngine 3.0.0
        */
        inline NCount getInput() const{ return getEditbox()->getInput();}

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getSelectBegin() const{ return getEditbox()->getSelectBegin();}

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getSelectEnd() const{ return getEditbox()->getSelectEnd();}

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getSelectLength() const{ return getEditbox()->getSelectLength(); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getMaxLength() const{ return getEditbox()->getMaxLength();}

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getItemCount() const{ return getListBox()->getItemCount(); }

        /**
        @version NIIEngine 3.0.0
        */
        inline ListboxItem * getSelectItem() const{ return getListBox()->getFirstSelect(); }

        /**
        @version NIIEngine 3.0.0
        */
        inline ListboxItem * getItem(NCount index) const{ return getListBox()->getItem(index);}

        /** 
        @version NIIEngine 3.0.0
        */
        inline Nidx getItemIndex(const ListboxItem * item) const{ return getListBox()->getItemIndex(item); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setSortEnable(bool set){ getListBox()->setSortEnable(set); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isSortEnable() const{ return getListBox()->isSortEnable(); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isItemSelect(NCount index) const{ return getListBox()->isItemSelect(index); }

        /** 
        @version NIIEngine 3.0.0
        */
        inline ListboxItem * getItem(const String & text, const ListboxItem * begin){ return getListBox()->getItem(text, begin);}

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isItemExist(const ListboxItem * item) const{ return getListBox()->isItemExist(item); }

        /**
        @version NIIEngine 3.0.0
        */
        void activateEditbox();
        
        /**
        @version NIIEngine 3.0.0
        */
        void deactivateEditbox();
        
        /**
        @version NIIEngine 3.0.0
        */
        void showDropList();

        /**
        @version NIIEngine 3.0.0
        */
        void hideDropList();

        /** 
        @version NIIEngine 3.0.0
        */
        inline void setReadOnly(bool set){ getEditbox()->setReadOnly(set); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setRegexString(const String & str){ getEditbox()->setRegexString(str); }

        /**
        @param idx
        @version NIIEngine 3.0.0
        */
        inline void setInput(NCount idx){ getEditbox()->setInput(idx); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setSelect(NCount begin, NCount end) { getEditbox()->setSelect(begin, end); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setSelectBegin(NCount idx) { getEditbox()->setSelectBegin(idx); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setSelectLength(NCount length) { getEditbox()->setSelectLength(length); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void setMaxLength(NCount length){ getEditbox()->setMaxLength(length); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void addItem(ListboxItem * item){ getListBox()->addItem(item); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void insertItem(ListboxItem * item, const ListboxItem * pos){ getListBox()->insertItem(item, pos);}

        /**
        @version NIIEngine 3.0.0
        */
        inline void removeItem(const ListboxItem * item){ getListBox()->removeItem(item); }
        
        /**
        @version NIIEngine 3.0.0
        */
        inline void removeAll(){ getListBox()->removeAllItem(); }

        /**
        @version NIIEngine 3.0.0
        */
        void setItemSelect(ListboxItem * item, bool state);

        /**
        @version NIIEngine 3.0.0
        */
        void setItemSelect(NCount idx, bool state);

        /**
        @version NIIEngine 3.0.0
        */
        inline void unselectAll(){ getListBox()->unselectAll(); }

        /**
        @version NIIEngine 3.0.0
        */
        inline void notifyItemUpdate(){ getListBox()->notifyItemUpdate(); }
        
        /// @copydetails Widget::intersects
        bool intersects(const Vector2f & pos, bool disable = false) const;
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReadOnlyModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID RegexStringEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MaxLengthEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID TextValidChangeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID InputPosEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SelectEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID InputFullEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID InputAcceptEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ItemListEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ItemSelectEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SortModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID VScrollModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID HScrollModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ListShowEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ListRemoveEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID SelectAcceptEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /**
        @version NIIEngine 3.0.0
        */
        void listButtonPressPrc(const EventArgs * e);

        /**
        @version NIIEngine 3.0.0
        */
        void listAcceptPrc(const EventArgs * e);

        /**
        @version NIIEngine 3.0.0
        */
        void listHidePrc(const EventArgs * e);

        /**
        @version NIIEngine 3.0.0
        */
        void editboxPressPrc(const EventArgs * e);
        
        /**
        @version NIIEngine 3.0.0
        */
        void editboxTextPrc(const EventArgs * e);
        
        /**
        @verison NIIEngine 3.0.0
        */
        void updateItemState(const ListboxItem * item, bool nstate, bool ostate);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onReadOnlyMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onRegexString(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onMaxLength(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onTextValid(const RegexMatchStateEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onInputPos(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onTextSelect(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onTextFull(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onTextAccept(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSortMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onVScrollbarMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onHScrollbarMode(const WidgetEventArgs * arg);
        
        /**
        @version NIIEngine 3.0.0
        */
        virtual void onItemList(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onListSelect(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onListShow(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onListRemove(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onSelectAccept(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onFont(const FontEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onText(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onActivate(const ActivationEventArgs * arg);

        /// @copydetails Container::onSize
        void onSize(const WidgetEventArgs * arg);
    protected:
        bool mAutoDropDown;
        bool mAutoHeight;
        bool mAutoWidth;
    };
}
}
#endif