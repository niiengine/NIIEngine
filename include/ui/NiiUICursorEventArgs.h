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
#include "NiiMouseControlPattern.h"

using namespace NII::NII_MEDIA;

namespace NII
{
namespace UI
{
    /** UI专用游标事件参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CursorEventArgs : public WidgetEventArgs
    {
    public:
        CursorEventArgs(Widget * w) :
            WidgetEventArgs(w),
            mCount(0){}
        CursorEventArgs(const MouseArgs * a, Widget * w) :
            mMouseArg(a),
            WidgetEventArgs(w){}

        CursorEventArgs(MouseControlPattern * src, Widget * w, MouseButton b,
            Ni32 x, Ni32 y, Ni32 scroll, NIIf lx, NIIf ly) :
            mMouseArg(src, x, y, scroll, b),
            WidgetEventArgs(w),
            mLocalX(lx),
            mLocalY(ly),
            mCount(0) {}
    public:
        MouseArgs mMouseArg;///<
        NIIf mLocalX;       ///< 
        NIIf mLocalY;       ///<
        NCount mCount;      ///< 点击次数
    };
}
}
#endif