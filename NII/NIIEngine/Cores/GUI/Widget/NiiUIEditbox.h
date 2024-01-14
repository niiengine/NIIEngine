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

#ifndef _NII_UI_EDITBOX_H_
#define _NII_UI_EDITBOX_H_

#include "NiiUIPreInclude.h"
#include "NiiRegexManager.h"
#include "NiiUIWidget.h"
#include "NiiClipboard.h"

namespace NII
{
namespace UI
{
    /** 文本编辑框
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Editbox : public Widget
    {
    public:
        typedef RegexMatcher::MatchState MatchState;

        Editbox(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        virtual ~Editbox();

        /** 设置是否使用掩码代替可见文字
        @remark 可以用于密码文本等效果
        @version NIIEngine 3.0.0
        */
        void setMarkMode(bool b);

        /** 返回是否使用掩码代替可见文字
        @remark 可以用于密码文本等效果
        @version NIIEngine 3.0.0
        */
        inline bool isMarkMode() const{ return mMarkMode; }

        /** 设置是否只读
        @version NIIEngine 3.0.0
        */
        void setReadOnly(bool b);

        /** 返回是否只读
        @version NIIEngine 3.0.0
        */
        bool isReadOnly() const;

        /**
        @version NIIEngine 3.0.0
        */
        void setRegex(RegexMatcher * matcher);

        /** 设置正则表达式匹配串
        @remark 请查看具体使用的第三方库的匹配说明
        @param string 正则表达式匹配串
        @version NIIEngine 3.0.0
        */
        void setRegexString(const String & string);

        /** 设置这个编辑框当前的字符串
        @remark 请查看具体使用的第三方库的匹配说明
        @return 正则表达式匹配串
        @version NIIEngine 3.0.0
        */
        inline const String & getRegexString() const{ return mRegexString;}

        /** 设置输入位置
        @version NIIEngine 3.0.0
        */
        void setInput(NCount index);

        /** 返回输入位置
        @version NIIEngine 3.0.0
        */
        NCount getInput() const;

        /** 
        @version NIIEngine 3.0.0
        */
        void setSelect(NCount begin, NCount end);

        /** 
        @version NIIEngine 3.0.0
        */
        void setSelectBegin(NCount idx);

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
        inline NCount getSelectLength() const{ return mSelectEnd - mSelectBegin; }

        /**
        @version NIIEngine 3.0.0
        */
        void setMarkChar(Nui32 code);

        /**
        @version NIIEngine 3.0.0
        */
        inline Nui32 getMarkChar() const{ return mMarkChar; }

        /**
        @version NIIEngine 3.0.0
        */
        void setMaxLength(NCount len);

        /** 
        @version NIIEngine 3.0.0
        */
        inline NCount getMaxLength() const{ return mMaxTextLength; }

        /// @copydetails Widget::onCopyData
        virtual bool onCopyData(Clipboard & obj);

        /// @copydetails Widget::onCutData
        virtual bool onCutData(Clipboard & obj);

        /// @copydetails Widget::onPasteData
        virtual bool onPasteData(Clipboard & obj);
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /**
        @version NIIEngine 3.0.0
        */
        NCount getTextIndex(const Vector2f & pt) const;

        /**
        @version NIIEngine 3.0.0
        */
        void removeSelectText(bool deletetext = true);

        /**
        @version NIIEngine 3.0.0
        */
        MatchState getMatchState(const String & str) const;

        /** 
        @version NIIEngine 3.0.0
         */
        bool checkValid(const String & str);

        void onBackspaceKey();

        void onDeleteKey();

        void onCharLeft(Nui32 sysKeys);

        void onWordLeft(Nui32 sysKeys);

        void onCharRight(Nui32 sysKeys);

        void onWordRight(Nui32 sysKeys);

        void onHomeKey(Nui32 sysKeys);

        void onEndKey(Nui32 sysKeys);
    protected:
        /// @copydetail Widget::validImpl
        virtual bool validImpl(const WidgetView * style) const;

        /// @copydetails Widget::onButtonDown
        void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMultiTouch
        void onCursorMultiTouch(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        void onLost(const WidgetEventArgs * arg);

        void onKeyDown(const KeyEventArgs * arg);

        /// @copydetails Widget::onChar
        void onChar(const KeyEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onReadOnlyMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onMarkMode(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onMarkChar(const WidgetEventArgs * arg);

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
        virtual void onSelect(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onTextFull(const WidgetEventArgs * arg);

        /**
        @version NIIEngine 3.0.0
        */
        virtual void onTextAccept(const WidgetEventArgs * arg);

        /// @copydtails Widget::onText
        virtual void onText(const WidgetEventArgs * arg);
    public:
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID ReadOnlyModeEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID MarkModeEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID MarkCharChangeEvent;
        
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
        static const EventID SelectChangeEvent;
        
        /** 
        @version NIIEngine 3.0.
        */
        static const EventID InputFullEvent;
        
        /** 
        @version NIIEngine 3.0.0
        */
        static const EventID InputAcceptEvent;

        /// 事件总数
        static const EventID EventCount;
    protected:
        Nui32 mMarkChar;
        NCount mMaxTextLength;
        NCount mInputPos;
        NCount mSelectBegin;
        NCount mSelectEnd;
        String mRegexString;
        RegexMatcher * mRegex;
        NCount mDragBeginPos;
        MatchState mRegexState;
        bool mPreStateChangeResponse;
        bool mDrag;             ///< 是否开始拖选
        bool mReadOnly;
        bool mMarkMode;         ///< 是否使用掩码模式(如密码)
        bool mCreateRegex;
    };

    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EditboxView : public WidgetView
    {
    public:
        EditboxView(WidgetViewlID wsid);

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
        void getPropertyColour(PropertyID pid, ColourArea & out) const;

        /** 
        @version NIIEngine 3.0.0
        */
        void setBlinkEnable(bool set);

        /** 
        @version NIIEngine 3.0.0
        */
        bool isBlinkEnable() const;

        /** 
        @version NIIEngine 3.0.0
        */
        void setBlinkTimeout(TimeDurMS ms);

        /** 
        @version NIIEngine 3.0.0
        */
        TimeDurMS getBlinkTimeout() const;

        /** 
        @version NIIEngine 3.0.0
        */
        void setTextLayout(HTextLayout format);

        /** 
        @version NIIEngine 3.0.0
        */
        HTextLayout getTextLayout() const;

        /// @copydetails WidgetView::update
        void update(TimeDurMS ms);

        /// @copydetails EditboxView::flush
        void flush();

        /// @copydetails EditboxView::getTextIndex
        NCount getTextIndex(const Vector2f & pt) const;
    protected:
        bool notifyFont(const Font * obj);
    public:
        /** ReadOnly/只读 标签中的元素
        @version NIIEngine 3.0.0
        */
        static const StateID ReadOnlyState;

        /**
        @version NIIEngine 3.0.0
        */
        static const StateID SelectColourState;

        /**
        @version NIIEngine 3.0.0
        */
        static const StateID UnselectColourState;

        /**
        @version NIIEngine 3.0.0
        */
        static const StateID ItemState;

        static const StateID StateCount;
    protected:
        /**
        @version NIIEngine 3.0.0
        */
        NCount getInput(const String & visual_string) const;

        /**
        @version NIIEngine 3.0.0
        */
        void render(const StyleSection & section, const Rectf & area, NIIf offset, NIIf extent_to_caret) const;
        
        /**
        @version NIIEngine 3.0.0
        */
        NIIf calcTextOffset(const Rectf & area, const NIIf text_extent, const NIIf caret_width, const NIIf extent_to_caret);

        /**
        @version NIIEngine 3.0.0
        */
        void renderTextNoBidi(const Style & style, const String & text, const Rectf & area, NIIf offset);

        /**
        @version NIIEngine 3.0.0
        */
        void renderTextBidi(const Style & style, const String & text, const Rectf & area,  NIIf offset);
    protected:
        NIIf mTextOffset;
        TimeDurMS mBlinkCost;
        TimeDurMS mBlinkTimeout;
        HTextLayout mTextLayout;
        bool mBlink;
        bool mBlinkEnable;
    };
}
}
#endif