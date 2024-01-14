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

#ifndef _NII_AnimationEventArgs_H_
#define _NII_AnimationEventArgs_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"

namespace NII
{
    /** ¶¯»­ÊÂ¼þ
    @version NIIEngine 3.0.0
    */
    class _EngineAPI AnimationEventArgs : public EventArgs
    {
    public:
        AnimationEventArgs(AnimationFusion * obj) : mFusion(obj) {}

        AnimationFusion * mFusion;
    };
}

#endif