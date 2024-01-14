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

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif

#include "NiiUI"
#include "CEGUI/ScriptModules/Lua/ScriptModule.h"
#include "CEGUI/ScriptModules/Lua/Functor.h"
#include <vector>

// include Lua libs and tolua++
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "tolua++.h"

// prototype for bindings initialisation function
int tolua_CEGUI_open(lua_State * tolua_S);

namespace NII
{
    //------------------------------------------------------------------------
    LuaScriptModule::LuaScriptModule(lua_State * state) :
        d_ownsState(state == 0),
        d_state(state),
        d_errFuncIndex(LUA_NOREF),
        d_activeErrFuncIndex(LUA_NOREF)
    {
        // initialise and create a lua_State if one was not provided
        if (!d_state)
        {
            #if CEGUI_LUA_VER >= 51
                static const luaL_Reg lualibs[] =
                {
                    {"", luaopen_base},
                    {LUA_LOADLIBNAME, luaopen_package},
                    {LUA_TABLIBNAME, luaopen_table},
                    {LUA_IOLIBNAME, luaopen_io},
                    {LUA_OSLIBNAME, luaopen_os},
                    {LUA_STRLIBNAME, luaopen_string},
                    {LUA_MATHLIBNAME, luaopen_math},
                #if defined(DEBUG) || defined (_DEBUG)
                        {LUA_DBLIBNAME, luaopen_debug},
                #endif
                    {0, 0}
                };
            #endif /* CEGUI_LUA_VER >= 51 */

            // create a lua state
            d_ownsState = true;
    #if LUA_VERSION_NUM > 501
            d_state = luaL_newstate();
    #else
            d_state = lua_open();
    #endif

            // init all standard libraries
            #if CEGUI_LUA_VER >= 51
                    const luaL_Reg *lib = lualibs;
                    for (; lib->func; lib++)
                    {
                        lua_pushcfunction(d_state, lib->func);
                        lua_pushstring(d_state, lib->name);
                        lua_call(d_state, 1, 0);
                    }
            #else /* CEGUI_LUA_VER >= 51 */
                luaopen_base(d_state);
                luaopen_io(d_state);
                luaopen_string(d_state);
                luaopen_table(d_state);
                luaopen_math(d_state);
                #if defined(DEBUG) || defined (_DEBUG)
                    luaopen_debug(d_state);
                #endif
            #endif /* CEGUI_LUA_VER >= 51 */
        }
        setModuleIdentifierString();
    }
    //------------------------------------------------------------------------
    LuaScriptModule::~LuaScriptModule()
    {
        if(d_state)
        {
            unrefErrorFunc();

            if(d_ownsState)
                lua_close(d_state);
        }
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::executeFile(const String & file, const String & group)
    {
        int top = lua_gettop(d_state);

        executeScriptFile_impl(file, group, initErrorHandlerFunc(), top);

        cleanupErrorHandlerFunc();
    }
    //------------------------------------------------------------------------
    int	LuaScriptModule::executeFunc(const String & function_name)
    {
        int top = lua_gettop(d_state);
        int r = executeScriptGlobal_impl(function_name, initErrorHandlerFunc(), top);
        cleanupErrorHandlerFunc();

        return r;
    }
    //------------------------------------------------------------------------
    bool LuaScriptModule::executeFunc(const String & handler_name,
        const EventArgs & e)
    {
        int top = lua_gettop(d_state);
        bool r = executeScriptedEventHandler_impl(handler_name, e,
            initErrorHandlerFunc(), top);
        cleanupErrorHandlerFunc();

        return r;
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::executeScript(const String & str)
    {
        int top = lua_gettop(d_state);

        executeString_impl(str, initErrorHandlerFunc(), top);
        cleanupErrorHandlerFunc();
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::load(void)
    {
        N_Only(Log).log(_T("---- Creating Lua bindings ----"));
        // init CEGUI module
        tolua_CEGUI_open(d_state);
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::unload(void)
    {
        N_Only(Log).log(_T("---- Destroying Lua bindings ----"));
        // is this ok ?
        lua_pushnil(d_state);
        lua_setglobal(d_state,"CEGUI");
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::setModuleIdentifierString()
    {
        // set ID string
        mID = "LuaScriptModule - Official Lua based scripting module for CEGUI";
    }
    //------------------------------------------------------------------------
    SignalPtr LuaScriptModule::add(CommandObj * target, EventID id, const String & fc)
    {
        const String & err_str = getActivePCallErrorHandlerString();
        const int err_ref = getActivePCallErrorHandlerReference();

        SignalPtr con;
        // do the real subscription
        if (err_ref == LUA_NOREF)
        {
            LuaFunctor functor(d_state, fc, LUA_NOREF, err_str);
            con = target->add(id, EventFunctor(functor));
            functor.invalidateLuaRefs();
        }
        else
        {
            LuaFunctor functor(d_state, fc, LUA_NOREF, err_ref);
            con = target->add(id, EventFunctor(functor));
            functor.invalidateLuaRefs();
        }

        // return the event connection
        return con;
    }
    //------------------------------------------------------------------------
    SignalPtr LuaScriptModule::add(CommandObj * target,
        EventID id, Event::Group group, const String & fc)
    {
        const String & err_str = getActivePCallErrorHandlerString();
        const int err_ref = getActivePCallErrorHandlerReference();

        SignalPtr con;
        // do the real subscription
        if(err_ref == LUA_NOREF)
        {
            LuaFunctor functor(d_state, fc, LUA_NOREF, err_str);
            con = target->add(id, group, EventFunctor(functor));
            functor.invalidateLuaRefs();
        }
        else
        {
            LuaFunctor functor(d_state, fc, LUA_NOREF, err_ref);
            con = target->add(id, group, EventFunctor(functor));
            functor.invalidateLuaRefs();
        }

        // return the event connection
        return con;
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::setDefaultPCallErrorHandler(const String & error_handler_function)
    {
        unrefErrorFunc();

        d_errFuncName = error_handler_function;
        d_errFuncIndex = LUA_NOREF;
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::setDefaultPCallErrorHandler(int function_reference)
    {
        unrefErrorFunc();

        d_errFuncIndex = function_reference;
        d_errFuncName.clear();
    }
    //------------------------------------------------------------------------
    const String & LuaScriptModule::getActivePCallErrorHandlerString() const
    {
        if ((d_activeErrFuncIndex == LUA_NOREF) && d_activeErrFuncName.empty())
            return d_errFuncName;
        else
            return d_activeErrFuncName;
    }
    //------------------------------------------------------------------------
    int LuaScriptModule::getActivePCallErrorHandlerReference() const
    {
        if ((d_activeErrFuncIndex == LUA_NOREF) && d_activeErrFuncName.empty())
            return d_errFuncIndex;
        else
            return d_activeErrFuncIndex;
    }
    //------------------------------------------------------------------------
    int LuaScriptModule::initErrorHandlerFunc()
    {
        d_activeErrFuncName = d_errFuncName;

        // should we create a registry reference for named function
        if ((d_errFuncIndex == LUA_NOREF) && !d_errFuncName.empty())
        {
            int top = lua_gettop(d_state);

            LuaFunctor::pushNamedFunction(d_state, d_errFuncName);
            // reference function
            d_errFuncIndex = luaL_ref(d_state, LUA_REGISTRYINDEX);

            lua_settop(d_state, top);
        }

        // init handler via function index in registry
        return initErrorHandlerFunc(d_errFuncIndex);
    }
    //------------------------------------------------------------------------
    int LuaScriptModule::initErrorHandlerFunc(const String func_name)
    {
        d_activeErrFuncName = func_name;

        // string has some text in it?
        if (!func_name.empty())
        {
            LuaFunctor::pushNamedFunction(d_state, func_name);
            return lua_gettop(d_state);
        }

        // use no error handler function.
        return 0;
    }
    //------------------------------------------------------------------------
    int LuaScriptModule::initErrorHandlerFunc(int func)
    {
        d_activeErrFuncIndex = func;

        // check if we have a valid registry index for the error handler function
        if (func != LUA_NOREF)
        {
            lua_rawgeti(d_state, LUA_REGISTRYINDEX, func);
            return lua_gettop(d_state);
        }

        // use no error handler function.
        return 0;
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::cleanupErrorHandlerFunc()
    {
        d_activeErrFuncIndex = LUA_NOREF;
        d_activeErrFuncName.clear();
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::unrefErrorFunc()
    {
        if((d_errFuncIndex != LUA_NOREF) && !d_errFuncName.empty())
        {
            luaL_unref(d_state, LUA_REGISTRYINDEX, d_errFuncIndex);
            d_errFuncIndex = LUA_NOREF;
        }
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::executeFile(const String & file, GroupID gid, 
        const String & error_handler)
    {
        int top = lua_gettop(d_state);

        executeScriptFile_impl(file, gid, initErrorHandlerFunc(error_handler), top);
        cleanupErrorHandlerFunc();
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::executeFile(const String & file, GroupID gid, 
        const int error_handler)
    {
        int top = lua_gettop(d_state);

        executeScriptFile_impl(file, gid, initErrorHandlerFunc(error_handler), top);
        cleanupErrorHandlerFunc();
    }
    //------------------------------------------------------------------------
    int LuaScriptModule::executeFunc(const String & function_name,
        const String & error_handler)
    {
        int top = lua_gettop(d_state);
        int r = executeScriptGlobal_impl(function_name,
            initErrorHandlerFunc(error_handler), top);
        cleanupErrorHandlerFunc();

        return r;
    }
    //------------------------------------------------------------------------
    int LuaScriptModule::executeFunc(const String & function_name,
        int error_handler)
    {
        int top = lua_gettop(d_state);
        int r = executeScriptGlobal_impl(function_name,
            initErrorHandlerFunc(error_handler), top);
        cleanupErrorHandlerFunc();

        return r;
    }
    //------------------------------------------------------------------------
    bool LuaScriptModule::executeFunc(const String & handler_name,
        const EventArgs & e, const String & error_handler)
    {
        int top = lua_gettop(d_state);

        bool r =executeScriptedEventHandler_impl(handler_name, e,
            initErrorHandlerFunc(error_handler), top);
        cleanupErrorHandlerFunc();

        return r;
    }
    //------------------------------------------------------------------------
    bool LuaScriptModule::executeFunc(const String & handler_name,
        const EventArgs& e, const int error_handler)
    {
        int top = lua_gettop(d_state);
        bool r = executeScriptedEventHandler_impl(handler_name, e,
            initErrorHandlerFunc(error_handler), top);
        cleanupErrorHandlerFunc();

        return r;
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::executeScript(const String & str, const String & error_handler)
    {
        int top = lua_gettop(d_state);

        executeString_impl(str, initErrorHandlerFunc(error_handler), top);
        cleanupErrorHandlerFunc();
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::executeScript(const String & str, const int errorfc)
    {
        int top = lua_gettop(d_state);

        executeString_impl(str, initErrorHandlerFunc(errorfc), top);
        cleanupErrorHandlerFunc();
    }
    //------------------------------------------------------------------------
    SignalPtr LuaScriptModule::add(CommandObj * target, EventID id,
        const String & fc, const String & errorfc)
    {
        // do the real subscription
        LuaFunctor functor(d_state, fc, LUA_NOREF, errorfc);
        SignalPtr con = target->add(id, EventFunctor(functor));

        // make sure we don't release the references we just made when 'functor' is
        // destroyed as it goes out of scope.
        functor.index = LUA_NOREF;
        functor.d_errFuncIndex = LUA_NOREF;

        // return the event connection
        return con;
    }
    //------------------------------------------------------------------------
    SignalPtr LuaScriptModule::add(CommandObj * target, EventID id,
        const String & fc, const int errorfc)
    {
        // do the real subscription
        LuaFunctor functor(d_state, fc, LUA_NOREF, errorfc);
        SignalPtr con =
            target->add(id, EventFunctor(functor));

        // make sure we don't release the references we just made when 'functor' is
        // destroyed as it goes out of scope.
        functor.index = LUA_NOREF;
        functor.d_errFuncIndex = LUA_NOREF;

        // return the event connection
        return con;
    }
    //------------------------------------------------------------------------
    SignalPtr LuaScriptModule::add(CommandObj * target, EventID id,
        Event::Group group, const String & fc, const String & errorfc)
    {
        // do the real subscription
        LuaFunctor functor(d_state, fc, LUA_NOREF, errorfc);
        SignalPtr con = target->add(id, group, EventFunctor(functor));

        // make sure we don't release the references we just made when 'functor' is
        // destroyed as it goes out of scope.
        functor.index = LUA_NOREF;
        functor.d_errFuncIndex = LUA_NOREF;

        // return the event connection
        return con;
    }
    //------------------------------------------------------------------------
    SignalPtr LuaScriptModule::add(CommandObj * target, EventID id,
        Event::Group group, const String & fc, const int errorfc)
    {
        // do the real subscription
        LuaFunctor functor(d_state, fc, LUA_NOREF, errorfc);
        SignalPtr con = target->add(id, group, EventFunctor(functor));

        // make sure we don't release the references we just made when 'functor' is
        // destroyed as it goes out of scope.
        functor.index = LUA_NOREF;
        functor.d_errFuncIndex = LUA_NOREF;

        // return the event connection
        return con;
    }
    //------------------------------------------------------------------------
    void LuaScriptModule::executeScriptFile_impl(const String & file, GroupID gid,
        const int err_idx, const int top)
    {
        // load file
        DataStream * data = N_Only(ResourceScheme).open(file, gid == 0 ? mResourceGroup : gid);

        // load code into lua
        int loaderr = luaL_loadbuffer(d_state, reinterpret_cast<char *>(data.getDate()),
            data.getSize(), file.c_str());

        N_delete data;
        if(loaderr)
        {
            String errMsg = lua_tostring(d_state,-1);
            lua_settop(d_state,top);
            N_EXCEPT(Script, _T("Unable to execute Lua script file: '") +
                file + "'\n\n" + errMsg + "\n"));
        }

        // call it
        if(lua_pcall(d_state, 0, 0, err_idx))
        {
            String errMsg = lua_tostring(d_state,-1);
            lua_settop(d_state,top);
            N_EXCEPT(Script, _T("Unable to execute Lua script file: '") +
                file + "'\n\n" + errMsg + "\n"));
        }

        lua_settop(d_state,top); // just in case :P
    }
    //------------------------------------------------------------------------
    int LuaScriptModule::executeScriptGlobal_impl(const String & function_name,
        const int err_idx, const int top)
    {
        // get the function from lua
        lua_getglobal(d_state, function_name.c_str());

        // is it a function
        if (!lua_isfunction(d_state,-1))
        {
            lua_settop(d_state,top);
            N_EXCEPT(Script, _T("Unable to get Lua global: '") +
                function_name + _T("' as name not represent a global Lua function" ));
        }

        // call it
        int error = lua_pcall(d_state, 0, 1, err_idx);

        // handle errors
        if (error)
        {
            String errMsg = lua_tostring(d_state,-1);
            lua_settop(d_state,top);
            N_EXCEPT(Script, _T("Unable to evaluate Lua global: '") +
                function_name + "\n\n" + errMsg + "\n"));
        }

        // get return value
        if (!lua_isnumber(d_state,-1))
        {
            // log that return value is invalid. return -1 and move on.
            lua_settop(d_state,top);
            N_EXCEPT(Script, _T("Unable to get Lua global : '") + function_name +
                _T("' return value as it's not a number"));
            return -1;
        }

        int ret = static_cast<int>(lua_tonumber(d_state,-1));
        lua_settop(d_state,top);

        // return it
        return ret;
    }
    //-----------------------------------------------------------------------
    bool LuaScriptModule::executeScriptedEventHandler_impl(const String & handler_name,
        const EventArgs & e, const int err_idx, const int top)
    {
        LuaFunctor::pushNamedFunction(d_state, handler_name);

        // push EventArgs as the first parameter
        tolua_pushusertype(d_state, (void*)&e, "const EventArgs");

        // call it
        int error = lua_pcall(d_state, 1, 1, err_idx);

        // handle errors
        if (error)
        {
            String errStr(lua_tostring(d_state,-1));
            lua_settop(d_state,top);

            N_EXCEPT(Script, _T("Unable to evaluate the Lua event handler: '") +
                handler_name + "'\n\n" + errStr + "\n"));
        }

        // retrieve result
        bool ret = lua_isboolean(d_state, -1) ? lua_toboolean(d_state, -1 ) : true;
        lua_settop(d_state,top);

        return ret;
    }
    //-----------------------------------------------------------------------
    void LuaScriptModule::executeString_impl(const String& str, const int err_idx,
        const int top)
    {
        // load code into lua and call it
        int error = luaL_loadbuffer(d_state, str.c_str(), str.length(), str.c_str()) ||
                    lua_pcall(d_state, 0, 0, err_idx);

        // handle errors
        if (error)
        {
            String errMsg = lua_tostring(d_state,-1);
            lua_settop(d_state,top);
            N_EXCEPT(Script, _T("Unable to execute Lua script string: '") +
                str + "'\n\n" + errMsg + "\n"));
        }

        lua_settop(d_state,top);
    }
    //-----------------------------------------------------------------------
    LuaScriptModule * LuaScriptModule::create(lua_State * state)
    {
        return new LuaScriptModule(state);
    }
    //-----------------------------------------------------------------------
    void LuaScriptModule::destroy(LuaScriptModule * mod)
    {
        delete mod;
    }
    //------------------------------------------------------------------------
}