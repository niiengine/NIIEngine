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

#ifndef _NII_UI_WIDGET_EVENTARGS_H_
#define _NII_UI_WIDGET_EVENTARGS_H_

#include "NiiUIPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiRegexManager.h"
#include "NiiPlaneSize.h"
#include "NiiKeyBoardControlPattern.h"

namespace NII
{
namespace UI
{
    /** UI元素属性发生变化时的参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI WidgetEventArgs : public EventArgs
    {
    public:
        WidgetEventArgs() : mWidget(0){}
        WidgetEventArgs(Widget * w) : mWidget(w){}
    public:
        mutable Widget * mWidget;
    };

    /** UI元素画盘发生变化时的参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SheetEventArgs : public WidgetEventArgs
    {
    public:
        SheetEventArgs(Widget * obj,  UISheet * sheet, UISubSheet * face) :
            WidgetEventArgs(obj),
            mSheet(sheet),
            mFace(face){}
    public:
        UISheet * mSheet;
        UISubSheet * mFace;
    };

    /** 视图窗体变化时的参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderSheetEventArgs : public SheetEventArgs
    {
    public:
        RenderSheetEventArgs(Widget * obj, UISheet * sheet, UISubSheet * face, ViewWindow * vwin) :
            SheetEventArgs(obj, sheet, face),
            mViewWindow(vwin){}

        ViewWindow * mViewWindow;
    };

    /** 正则表达式匹配时的参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RegexMatchStateEventArgs : public WidgetEventArgs
    {
    public:
        RegexMatchStateEventArgs(Widget * widget, RegexMatcher::MatchState state) :
            WidgetEventArgs(widget),
            mState(state){}

        RegexMatcher::MatchState mState;
    };
    
    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI UpdateEventArgs : public WidgetEventArgs
    {
    public:
        UpdateEventArgs(Widget * widget, TimeDurMS cost) :
            WidgetEventArgs(widget),
            mLastCost(cost){}

        TimeDurMS mLastCost;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CursorViewArgs : public EventArgs
    {
    public:
        CursorViewArgs(Cursor * cursor) : mCursor(cursor) {}

        Cursor * mCursor;
        const PixelBuffer * mImage;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI KeyEventArgs : public WidgetEventArgs
    {
    public:
        KeyEventArgs(Widget * widget) : WidgetEventArgs(widget) {}

        Ntchar mCodePoint;
        NII_MEDIA::KeyValue mScanCode;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ActivationEventArgs : public WidgetEventArgs
    {
    public:
        ActivationEventArgs(Widget * widget) : WidgetEventArgs(widget) {}

        Widget * mActWidget;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DragDropEventArgs : public WidgetEventArgs
    {
    public:
        DragDropEventArgs(Widget * widget) : WidgetEventArgs(widget) {}
        DragContainer * mItem;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DisplayEventArgs : public EventArgs
    {
    public:
        DisplayEventArgs(const PlaneSizei & sz):
            mSize(sz){}

        PlaneSizei mSize;
    };

    /**
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ResourceEventArgs : public EventArgs
    {
    public:
        ResourceEventArgs(const String & type, const String & name) :
           mType(type),
           mName(name){}

        String mType;
        String mName;
    };
}
}
#endif