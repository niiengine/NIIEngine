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
#ifndef _NII_UI_MultiLineEditbox_H_
#define _NII_UI_MultiLineEditbox_H_

#include "NiiUIPreInclude.h"
#include "NiiUIContainer.h"
#include "NiiClipboard.h"

namespace NII
{
namespace UI
{
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MultiEditbox : public Container
    {
    public:
        /**
        @version NIIEngine 3.0.0
        */
        struct LineInfo
        {
            NCount mCharIndex;
            NCount mLength;
            NIIf   mWidth;
        };
        typedef vector<LineInfo>::type LineList;
    public:
        MultiEditbox(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~MultiEditbox();

        /** 
        @version NIIEngine 3.0.0
        */
        void setReadOnly(bool set);

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isReadOnly() const {return mReadOnly;}

        /** 
        @version NIIEngine 3.0.0
        */
        void setInput(NCount idx);

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getInput() const {return d_caretPos;}

        /** 
        @version NIIEngine 3.0.0
        */
        void setMaxLength(NCount size);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getMaxLength() const {return mMaxLength; }
        
        /** 
        @version NIIEngine 3.0.0
        */
        void setWordWrap(bool set);

        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isWordWrap() const{ return mWordWrap; }
        
        /**
        @version NIIEngine 3.0.0
        */
        void SetForceVScrollbar(bool set);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline bool isForceVScrollbar() const { return mForceVScroll; }

        /**
        @version NIIEngine 3.0.0
        */
        Scrollbar * getVScrollbar() const;

        /**
        @version NIIEngine 3.0.0
        */
        Scrollbar * getHScrollbar() const;

        /** @version NIIEngine 3.0.0
        */
        void setSelect(Nidx begin, Nidx end);

        /** 
        @version NIIEngine 3.0.0
        */
        void setSelectBegin(NCount begin);
        
        /** 
        @version NIIEngine 3.0.0
        */
        NCount getSelectBegin() const;

        /** 
        @version NIIEngine 3.0.0
        */
        NCount getSelectEnd() const;        

        /** 
        @version NIIEngine 3.0.0
        */
        void setSelectLength(NCount length);
        
        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getSelectLength() const { return mSelectEnd - mSelectBegin;}
        
        /** 
        @version NIIEngine 3.0.0
        */
        Rectf getTextRenderArea() const;
        
        /** 
        @version NIIEngine 3.0.0
        */
        NCount getCharLine(NCount charpos) const;

        /**
        @version NIIEngine 3.0.0
        */
        inline const LineList & getLineList() const {return mLineList;}

        /**
        @version NIIEngine 3.0.0
        */
        void visibleChar(NCount charpos);
        
        /**
        @version NIIEngine 3.0.0
        */
        void formatText(bool udpateview);

        /**
        @version NIIEngine 3.0.0
        */
        void setSelectView(PixelBuffer * pb);

        /**
        @version NIIEngine 3.0.0
        */
        inline PixelBuffer * getSelectView() const{ return mViewBrush;}

        /// @copydetails Widget::onCopyData
        virtual bool onCopyData(Clipboard & obj);

        /// @copydetails Widget::onCutData
        virtual bool onCutData(Clipboard & obj);

        /// @copydetails Widget::onPasteData
        virtual bool onPasteData(Clipboard & obj);
    protected:
        /// @copydetails Container::initChild
        virtual void initChild();

        /// @copydetails PropertyCmdObj::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /** 
        @version NIIEngine 3.0.0
        */
        void updateScrollbar();

        /** 
        @version NIIEngine 3.0.0
        */
        NCount getTextIndex(const Vector2f & pt) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void removeSelectText(bool modify = true);

        /**
        @version NIIEngine 3.0.0
        */
        void onBackspaceKey();

        /**
        @version NIIEngine 3.0.0
        */
        void onDeleteKey();
        
        /** 
        @version NIIEngine 3.0.0
        */
        void onEnterKey(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onCharLeft(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onWordLeft(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onCharRight(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onWordRight(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onDocHome(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onDocEnd(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onLineHome(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onLineEnd(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onLineUp(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onLineDown(Nui32 key);

        /** 
        @version NIIEngine 3.0.0
        */
        void onPageUp(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void onPageDown(Nui32 key);

        /**
        @version NIIEngine 3.0.0
        */
        void ScrollPrc(const EventArgs * args);

        /**
        @version NIIEngine 3.0.0
        */
        void VScrollbarPrc(const EventArgs * arg);

        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * style) const;

        /**
        @version NIIEngine 3.0.0
        */
        void onReadOnlyMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        void onWordWrapMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        void onMaxLength(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        void onInputPos(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        void onTextSelect(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        void onTextFull(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        void onVScrollbarMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        void onHScrollbarMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onText(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onFont(const FontEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMultiTouch
        virtual void onCursorMultiTouch(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);

        /// @copydetails Widget::onChar
        virtual void onChar(const KeyEventArgs * arg);

        /// @copydetails Widget::onKeyDown
        virtual void onKeyDown(const KeyEventArgs * arg);

        /// @copydetails Container::onSize
        virtual void onSize(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorDrag
        virtual void onCursorDrag(const CursorEventArgs * arg);
    public:
        /**
        @version NIIEngine 3.0.0
        */
        static const EventID ReadOnlyModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID WordWrapModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID MaxLengthEvent;

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
        static const EventID VScrollModeEvent;

        /**
        @version NIIEngine 3.0.0
        */
        static const EventID HScrollModeEvent;

        static const EventID EventCount;
        
        static const WidgetID VScrollbarID;
        static const WidgetID HScrollbarID;
    protected:
        LineList mLineList;
        NCount mMaxLength;
        NCount d_caretPos;
        NCount mSelectBegin;
        NCount mSelectEnd;
        NCount d_dragAnchorIdx;
        NIIf d_widestExtent;
        PixelBuffer * mViewBrush;
        
        bool mForceVScroll;
        bool mForceHScroll;
        bool mWordWrap;
        bool mReadOnly;
        bool mDrag;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI MultiEditboxView : public WidgetView
    {
    public:
        MultiEditboxView(WidgetViewlID wmid);

        /**
        @version NIIEngine 3.0.0
        */
        virtual Rectf getRenderArea() const;

        /**
        @version NIIEngine 3.0.0
        */
        void update(TimeDurMS cost);

        /**
        @version NIIEngine 3.0.0
        */
        inline void setBlinkEnable(bool set){ mBlinkEnable = set; }

        /**
        @version NIIEngine 3.0.0
        */
        inline bool isBlinkEnable() const{ return mBlinkEnable;}

        /**
        @version NIIEngine 3.0.0
        */
        inline void setBlinkTimeout(TimeDurMS ms){ mBlinkTimeout = ms; }

        /**
        @version NIIEngine 3.0.0
        */
        inline TimeDurMS getBlinkTimeout() const{ return mBlinkTimeout; }
        
        /// @copydetails WidgetView::flush
        void flush();

        /// @copydetails WidgetView::notifyFont
        bool notifyFont(const Font * font);
    public:
        static const StateID ReadOnlyState;
        static const StateID ItemState;
        static const StateID StateCount;
    protected:
        /// @copydetails WidgetView::onStyleAttach
        void onStyleAttach();

        /** 
        @version NIIEngine 3.0.0
        */
        void renderLineList(const Rectf & area);

        /** 
        @version NIIEngine 3.0.0
        */
        void getUnselectColour(ColourArea & colour) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void getSelectColour(ColourArea & colour) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void getActiveSelectColour(ColourArea & colour) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void getInactiveSelectColour(ColourArea & colour) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void getPropertyColour(PropertyID pid, ColourArea & colour) const;
    protected:
        TimeDurMS mBlinkTimeout;
        TimeDurMS mBlinkCost;
        bool mBlinkEnable;
        bool mBlink;
    };
}
}
#endif