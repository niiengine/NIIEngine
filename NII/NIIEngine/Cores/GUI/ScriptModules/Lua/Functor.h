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

#ifndef _CEGUILuaFunctor_h_
#define _CEGUILuaFunctor_h_

#include "NiiCommandObj.h"

struct lua_State;

namespace CEGUI
{
    // forward declaration
    class LuaScriptModule;
    /**
    \brief Functor class used for subscribing Lua functions to CEGUI events
    */
    class LuaFunctor
    {
    public:
        LuaFunctor(lua_State * state, int func, int selfIndex);
        LuaFunctor(lua_State * state, const String & func, int selfIndex);

        LuaFunctor(lua_State * state, const int func, const int selfIndex,
            const String & error_handler);
        LuaFunctor(lua_State * state, const String & func, const int selfIndex,
            const String & error_handler);
        LuaFunctor(lua_State * state, const int func, const int selfIndex,
            const int error_handler);
        LuaFunctor(lua_State * state, const String & func, const int selfIndex,
            const int error_handler);

        LuaFunctor(const LuaFunctor & cp);
        ~LuaFunctor();

        bool operator()(const EventArgs & args) const;

        /**
        */
        static SignalPtr SubscribeEvent(CommandObj * self,
            const String & eventName, const int funcIndex, const int selfIndex,
                const int error_handler, lua_State * L);

        /**
        */
        static void pushNamedFunction(lua_State * L, const String & name);
    private:
        /**
        \brief
            Invalidate the registry references.  This is used internally to ensure
            that the references do not get released (for example when we destroy
            a temporary object)
        */
        void invalidateLuaRefs();

        lua_State * L;
        mutable int index;
        int self;
        mutable bool needs_lookup;
        mutable String function_name;

        /// Error handler function to pass to lua_pcall.
        mutable String d_errFuncName;
        /// registry index of the function to pass to lua_pcall.
        mutable int d_errFuncIndex;
        /// signfies whether we made the reference index at d_errFuncIndex.
        mutable bool d_ourErrFuncIndex;

        friend class LuaScriptModule;
    };
} // namespace CEGUI
#endif // end of guard _CEGUILuaFunctor_h_