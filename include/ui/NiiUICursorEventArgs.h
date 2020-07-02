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

#ifndef _NII_UI_CURSOR_EVENT_ARGS_H_
#define _NII_UI_CURSOR_EVENT_ARGS_H_

#include "NiiUIPreInclude.h"
#include "NiiUIWidgetEventArgs.h"
#include "NiiMouseControlItem.h"

using namespace NII::NII_MEDIA;

namespace NII
{
namespace UI
{
    /** UI专用游标事件参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CursorEventArgs : public MouseMoveBaseArgs, public MousePressBaseArgs,
        public WidgetEventArgs
    {
    public:
        CursorEventArgs(Widget * w) :
            WidgetEventArgs(w),
            mCount(0){}
        CursorEventArgs(const MouseMoveArgs * a, Widget * w) :
            MouseMoveBaseArgs(a),
            WidgetEventArgs(w){}
        CursorEventArgs(const MousePressArgs * a, Widget * w) :
            MousePressBaseArgs(a),
            WidgetEventArgs(w){}
        CursorEventArgs(const MouseMoveArgs * a1, const MousePressArgs * a2) :
            MouseMoveBaseArgs(a1),
            MousePressBaseArgs(a2),
            mCount(0){}
        CursorEventArgs(const MouseMoveArgs * a1, const MousePressArgs * a2, Widget * w) :
            MouseMoveBaseArgs(a1),
            MousePressBaseArgs(a2),
            WidgetEventArgs(w),
            mCount(0){}
        CursorEventArgs(MouseControlPattern * src, Widget * w, MouseButton b,
            Ni32 x, Ni32 y, Ni32 scroll, NIIf lx, NIIf ly) :
            MouseMoveBaseArgs(src, x, y, scroll),
            MousePressBaseArgs(src, b),
            WidgetEventArgs(w),
            mLocalX(lx),
            mLocalY(ly),
            mCount(0) {}
    public:
        NIIf mLocalX;       ///< 
        NIIf mLocalY;       ///<
        NCount mCount;      ///< 点击次数
    };
}
}
#endif