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

#include "NiiPreProcess.h"
#include "NiiVertexFusion.h"

namespace NII
{
    //------------------------------------------------------------------------
    VertexFusion::VertexFusion(AnimationObj * obj, Animation * ani, TimeDurMS pos,
        TimeDurMS dur, NIIf weight, bool enable) :
            AnimationFusion(obj, ani, pos, dur, weight, enable)
    {
    }
    //------------------------------------------------------------------------
    VertexFusion::VertexFusion(AnimationFusionObj * own, const VertexFusion & o):
        AnimationFusion(own, o)
    {
    }
    //------------------------------------------------------------------------
    VertexFusion::~VertexFusion()
    {
    }
    //------------------------------------------------------------------------
}