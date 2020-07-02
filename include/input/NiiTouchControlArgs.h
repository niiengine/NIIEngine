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

#ifndef _NII_TOUCH_CONTROL_ARGS_H_
#define _NII_TOUCH_CONTROL_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
namespace NII_MEDIA
{
    /** 触屏事件参数
    @version NIIEngine 3.0.0
    */
    class _EngineAPI TouchControlArgs : public EventArgs
    {
    public:
        TouchControlArgs(const Vector3<NIIi> & p, const Vector3<NIIi> & r) :
            pos(p),
            rel(r) {}
        
        const Vector3<NIIi> & pos;  ///< 当前位置
        const Vector3<NIIi> & rel;  ///< 当前位置和上次位置的差值
    };
}
}

#endif