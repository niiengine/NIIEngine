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
#ifndef _NII_RenderEffectManager_H_
#define _NII_RenderEffectManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiRenderEffectFactory.h"
#include "NiiLogManager.h"
#include "NiiStrConv.h"
#include "NiiException.h"

namespace NII
{
    /** 
    @version NIIEngine 3.0.0
    */
    class _EngineAPI RenderEffectManager : public Singleton<RenderEffectManager>, public UIAlloc
    {
    public:
        RenderEffectManager();
        ~RenderEffectManager();

        /** 
        */
        template <typename T> void addEffect(EffectID id);

        /** 
        */
        void removeEffect(EffectID id);

        /** 
        */
        bool isEffectAvailable(EffectID id) const;

        /** 
        */
        RenderEffect * create(EffectID id, UI::Window * win);

        /** 
        */
        void destroy(RenderEffect * obg);
    private:
        typedef map<EffectID, RenderEffectFactory *>::type RenderEffectRegistry;
        typedef map<RenderEffect *, RenderEffectFactory *, std::less<RenderEffect *>>::type EffectCreatorMap;

        RenderEffectRegistry mEffectFactoryList;
        EffectCreatorMap mEffectList;
    };
    //------------------------------------------------------------------------
    template <typename T> void RenderEffectManager::addEffect(EffectID name)
    {
        if(isEffectAvailable(name))
            N_EXCEPT(UniqueRepeat, _I18n("A RenderEffect is already registered under the name '") + N_conv(name) + _T("'"));

        mEffectFactoryList[name] = N_new TplRenderEffectFactory<T>;

        N_Only(Log).log(_I18n("Registered RenderEffect named '") + N_conv(name) + _T("'"));
    }
    //------------------------------------------------------------------------
}
#endif