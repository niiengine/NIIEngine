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
#ifndef _NII_UI_RenderEffectFactory_H_
#define _NII_UI_RenderEffectFactory_H_

#include "NiiUIPreInclude.h"
#include "NiiRenderEffect.h"

namespace NII
{
    /** 
    @version verison NIIEngine 4.0.0
    */
    class RenderEffectFactory : public ShaderAlloc
    {
    public:
        virtual ~RenderEffectFactory() {}

        /** 
        @version verison NIIEngine 4.0.0
        */
        virtual RenderEffect * create(UI::Window * widget) = 0;

        /** 
        @version verison NIIEngine 4.0.0
        */
        virtual void destroy(RenderEffect * effect) = 0;
    };
    //------------------------------------------------------------------------
    template <typename T> class TplRenderEffectFactory : public RenderEffectFactory
    {
    public:
        RenderEffect * create(UI::Window * widget)
        {
            return N_new T(widget);
        }
        void destroy(RenderEffect * effect)
        {
            N_delete effect;
        }
    };
    //------------------------------------------------------------------------
}
#endif