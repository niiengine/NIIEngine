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
#ifndef _NII_UI_TOGGLE_BUTTON_H_
#define _NII_UI_TOGGLE_BUTTON_H_

#include "NiiUIPreInclude.h"
#include "NiiUIButton.h"

namespace NII
{
namespace UI
{
    /** 切换按钮
    @remark 用于描述2种状态(on or off)的按钮,并在这2者间切换
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ToggleButton : public Button
    {
    public:
        ToggleButton(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);

        /** 设置状态
        @version NIIEngine 3.0.0
        */
        void on(bool b);

        /** 是否处于ON状态
        @version NIIEngine 3.0.0
        */
        bool isOn() const;
    public:
        /** 状态改变事件
        @version NIIEngine 3.0.0
        */
        static const EventID StateOnEvent;

        /** 事件总数
        @version NIIEngine 3.0.0
        */
        static const EventID EventCount;
    protected:
        /// @copydetails PropertyCmdObj::initCmd
        virtual bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// event triggered internally when toggle button on state changes.
        virtual void onStateOn(const WidgetEventArgs * arg);
    protected:
        bool mOn;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ToggleButtonView : public ButtonView
    {
    public:
        ToggleButtonView(WidgetViewlID wsid);

        static const StateID SelectEnableState;
        static const StateID SelecteHoverState;
        static const StateID SelectePushState;
        static const StateID SelectePushOffState;
        static const StateID SelecteDisableState;
        static const StateID StateCount;
    protected:
        StateID alter(StateID pid) const;
    };
}
}
#endif