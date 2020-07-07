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

#ifndef _NII_RENDEREFFECT_H_
#define _NII_RENDEREFFECT_H_

#include "NiiUIPreInclude.h"
#include "NiiUIInstance.h"
#include "NiiGeometryPixel.h"

namespace NII
{
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderEffect : public UIAlloc
    {
    public:
        virtual ~RenderEffect() {}

        /** 
        */
        virtual void onRenderBegin(NCount pass) = 0;

        /** 
        */
        virtual void onRenderEnd() = 0;
        
        /**
        */
        virtual bool onRefush(RenderSys * sys, GeometryPixel * geo) = 0;
        
        /**
        */
        virtual bool onUpdate(TimeDurMS cost, RenderSys * sys) = 0;
        
        /** 
        */
        virtual NCount getPassCount() const = 0;
    };
}
#endif