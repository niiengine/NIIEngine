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

#ifndef _NII_UI_BUTTON_H_
#define _NII_UI_BUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetView.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** 按钮型UI单元对象
    @remark 按钮等事物
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Button : public Widget
    {
    public:
        Button(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank, 
            ScriptTypeID stid = N_CmdObj_Button, LangID lid = N_PrimaryLang);

        virtual ~Button();

        /** 设置是否处于按下状态
        @version NIIEngine 3.0.0
        */
        void press(bool b);

        /** 设置是否处于选中状态
        @version NIIEngine 3.0.0
        */
        void select(bool b);

        /** 获取是否处于选中状态
        @version NIIEngine 3.0.0
        */
        bool isSelect() const;
        
        /** 游标是否在这个UI单元对象上方
        @version NIIEngine 3.0.0
        */
        bool isHover() const;

        /** 这个UI单元对象是否处于被按下状态
        @version NIIEngine 3.0.0
        */
        bool isPress() const;
    protected:
        /// @copydetails Button::onSelect
        virtual void onSelect(const WidgetEventArgs * arg);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorLeave
        virtual void onCursorLeave(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    public:
        /** 选中事件
        @version NIIEngine 3.0.0
        */
        static const EventID SelectChangeEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        ///@copydetails Widget::int;
        bool initCmd(PropertyCmdSet * dest);

        /** 更新UI单元对象
        @param pos UI体系中的位
        */
        void update(const Vector2f & cursorpos);
    protected:
        bool mPress;    ///< 是否处于被按下状态
        bool mOver;     ///< 游标覆盖状态下
        bool mSelect;
    };

    /** 默认按纽
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ButtonView : public WidgetView
    {
    public:
        ButtonView(WidgetModelID wsid);

        /// @copydetails WidgetView::flush
        virtual void flush();

        virtual StateID alter(StateID pid) const;
    public:
        static const StateID HoverState;
        static const StateID PushState;
        static const StateID PushOffState;
        static const StateID StateCount;
    };
}
}
#endif