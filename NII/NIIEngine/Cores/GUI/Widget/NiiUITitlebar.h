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
#ifndef _NII_UI_TITLEBAR_H_
#define _NII_UI_TITLEBAR_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidget.h"

namespace NII
{
namespace UI
{
    /** 标题栏
    @version NIIEngine 3.0.0
    */
    class _EngineAPI Titlebar : public Widget
    {
    public:
        Titlebar(WidgetID wid, FactoryID fid, Container * own, const String & name = N_StrBlank,
            LangID lid = N_PrimaryLang);
        virtual ~Titlebar();

        /** 设置是否可拖拽
        @version NIIEngine 3.0.0
        */
        void setDragEnable(bool b);

        /** 获取是否可拖拽
        @version NIIEngine 3.0.0
        */
        bool isDragEnable() const                   { return mDragEnable; }

        /** 是否拖拽中
        @version NIIEngine 3.0.0
        */
        bool isDragging() const                     { return mDrag; }

        /** 获取拖拽开始处
        @version NIIEngine 3.0.0
        */
        const Vector2f & getDragStartPos() const    { return mDragPos; }
    protected:
        /// @copydetails Titlebar::initCmd
        bool initCmd(PropertyCmdSet * dest);

        /// @copydetails Widget::onCursorMove
        virtual void onCursorMove(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonDown
        virtual void onButtonDown(const CursorEventArgs * arg);

        /// @copydetails Widget::onButtonUp
        virtual void onButtonUp(const CursorEventArgs * arg);

        /// @copydetails Widget::onCursorMultiTouch
        virtual void onCursorMultiTouch(const CursorEventArgs * arg);

        /// @copydetails Widget::onFont
        virtual void onFont(const FontEventArgs * arg);

        /// @copydetails Widget::onLost
        virtual void onLost(const WidgetEventArgs * arg);
    protected:
        Vector2f mDragPos;
        Recti mDragArea;
        bool mDragEnable;        
        bool mDrag;       
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TitlebarView : public WidgetView
    {
    public:
        TitlebarView(WidgetViewlID wsid);

        /// @copydetails WidgetView::flush
        void flush();
    public:
        static const StateID ActiveState;
        static const StateID InactiveState;
        static const StateID StateCount;
    };
}
}
#endif