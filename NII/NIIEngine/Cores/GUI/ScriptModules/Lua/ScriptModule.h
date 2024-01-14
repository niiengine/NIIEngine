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

#ifndef _CEGUILua_h_
#define _CEGUILua_h_

/*************************************************************************
	Import / Export control macros
*************************************************************************/
#if (defined( __WIN32__ ) || defined( _WIN32 )) && !defined(CEGUI_STATIC)
    #ifdef CEGUILUASCRIPTMODULE_EXPORTS
        #define CEGUILUA_API __declspec(dllexport)
    #else
        #define CEGUILUA_API __declspec(dllimport)
    #endif
#else
    #define CEGUILUA_API
#endif

#include "CEGUI/ScriptModule.h"

struct lua_State;

namespace CEGUI
{
    /** 
    @version NIIEngine 3.0.0
    */
    class CEGUILUA_API LuaScriptModule : public ScriptModule
    {
    public:
        /** Creates a LuaScriptModule object.
        @param state
        */
        static LuaScriptModule * create(lua_State * state = 0);

        /** 
        @version NIIEngine 3.0.0
        */
        static void destroy(LuaScriptModule * mod);

        /** Execute a script file.
        @param file
            String object holding the filename of the script file that is to be
            executed.
        @param gid
        */
        void executeFile(const String & file, GroupID gid);

        /** Execute a script file.
        @param file
            String object holding the filename of the script file that is to be
            executed.
        @param gid
        @param error_handler
            String containing the name of the script fuction that will be called
            if an error occurs while executing the handler function.  NB: This is
            the function passed as the error handler to lua_pcall.
        */
        void executeFile(const String & file, GroupID gid, const String & error_handler);

        /** Execute a script file.
        @param file
            String object holding the filename of the script file that is to be
            executed.
        @param gid
        @param error_handler
            integer value describing a lua registry reference to the function that
            will be called if an error occurs while executing the handler function.
            NB: This is the function passed as the error handler to lua_pcall.
        */
        void executeFile(const String & file, GroupID gid, const int error_handler);

        /** Execute a scripted global function.  The function should not take any
            parameters and should return an integer.
        @param function_name
            String object holding the name of the function, in the global script
            environment, that is to be executed.
        @return
            The integer value returned from the script function.
        */
        int executeFunc(const String & function_name);

        /** Execute a scripted global function.  The function should not take any
            parameters and should return an integer.
        @param function_name
            String object holding the name of the function, in the global script
            environment, that is to be executed.
        @param error_handler
            String containing the name of the script fuction that will be called
            if an error occurs while executing the handler function.  NB: This is
            the function passed as the error handler to lua_pcall.
        @return
            The integer value returned from the script function.
        */
        int executeFunc(const String & function_name, const String & error_handler);

        /** Execute a scripted global function.  The function should not take any
            parameters and should return an integer.
        @param function_name
            String object holding the name of the function, in the global script
            environment, that is to be executed.
        @param error_handler
            integer value describing a lua registry reference to the function that
            will be called if an error occurs while executing the handler function.
            NB: This is the function passed as the error handler to lua_pcall.
        @return
            The integer value returned from the script function.
        */
        int executeFunc(const String & function_name, int error_handler);

        /** Execute a scripted global 'event handler' function by looking it up by
            name.
        @param handler_name
            String object holding the name of the scripted handler function.
            If this string contains dots '.' it will be parsed as tables containing
            a function field.  For example: 'mytable.subtable.func'
        @param e
            EventArgs based object that should be passed, by any appropriate means,
            to the scripted function.
        @return
        */
        bool executeFunc(const String & handler_name, const EventArgs & e);

        /** Execute a scripted global 'event handler' function by looking it up by
            name.
        @param handler_name
            String object holding the name of the scripted handler function.
            If this string contains dots '.' it will be parsed as tables containing
            a function field.  For example: 'mytable.subtable.func'
        @param e
            EventArgs based object that should be passed, by any appropriate means,
            to the scripted function.
        @param error_handler
            String containing the name of the script fuction that will be called
            if an error occurs while executing the handler function.  NB: This is
            the function passed as the error handler to lua_pcall.
        */
        bool executeFunc(const String & handler_name,
            const EventArgs & e, const String & error_handler);

        /** Execute a scripted global 'event handler' function by looking it up by
            name.
        @param handler_name
            String object holding the name of the scripted handler function.
            If this string contains dots '.' it will be parsed as tables containing
            a function field.  For example: 'mytable.subtable.func'
        @param e
            EventArgs based object that should be passed, by any appropriate means,
            to the scripted function.
        @param error_handler
            integer value describing a lua registry reference to the function that
            will be called if an error occurs while executing the handler function.
            NB: This is the function passed as the error handler to lua_pcall.
        */
        bool executeFunc(const String & handler_name,
            const EventArgs & e, const int error_handler);

        /** Execute script code contained in the given String object.
        @param str
            String object holding the valid script code that should be executed.
        @return
            Nothing.
        */
        void executeScript(const String & str);

        /** Execute script code contained in the given String object.
        @param str
            String object holding the valid script code that should be executed.
        @param errorfc
            String containing the name of the script fuction that will be called
            if an error occurs while executing the handler function.  NB: This is
            the function passed as the error handler to lua_pcall.
        @return Nothing.
        */
        void executeScript(const String & str, const String & errorfc);

        /** Execute script code contained in the given String object.
        @param str
            String object holding the valid script code that should be executed.
        @param errorfc
            integer value describing a lua registry reference to the function that
            will be called if an error occurs while executing the handler function.
            NB: This is the function passed as the error handler to lua_pcall.
        @return
            Nothing.
        */
        void executeScript(const String & str, const int errorfc);

        /** Subscribes the named Event to a scripted funtion
        @param target
            The target CommandObj for the subscription.
        @param name
            String object containing the name of the Event to subscribe to.
        @param fc
            String object containing the name of the script funtion that is to be
            subscribed to the Event.
        @return
            Connection object that can be used to check the status of the Event
            connection and to disconnect (unsubscribe) from the Event.
        */
        SignalPtr add(CommandObj * target, EventID id, const String & fc);

        /** Subscribes the named Event to a scripted funtion
        @param target
            The target CommandObj for the subscription.
        @param id
            String object containing the name of the Event to subscribe to.
        @param functor
            String object containing the name of the script funtion that is to be
            subscribed to the Event.
        @param error_handler
            String containing the name of the script fuction that will be called
            if an error occurs while executing the handler function.  NB: This is
            the function passed as the error handler to lua_pcall.
        @return
            Connection object that can be used to check the status of the Event
            connection and to disconnect (unsubscribe) from the Event.
        */
        SignalPtr add(CommandObj * target, EventID id, const String & fc, const String & errorfc);

        /** Subscribes the named Event to a scripted funtion
        @param target
            The target CommandObj for the subscription.
        @param id
            String object containing the name of the Event to subscribe to.
        @param functor
            String object containing the name of the script funtion that is to be
            subscribed to the Event.
        @param error_handler
            integer value describing a lua registry reference to the function that
            will be called if an error occurs while executing the handler function.
            NB: This is the function passed as the error handler to lua_pcall.
        @return
            Connection object that can be used to check the status of the Event
            connection and to disconnect (unsubscribe) from the Event.
        */
        SignalPtr add(CommandObj * target, EventID id, const String & fc, const int errorfc);

        /** Subscribes the specified group of the named Event to a scripted funtion.
        @param target
            The target CommandObj for the subscription.
        @param id
            String object containing the name of the Event to subscribe to.
        @param group
            Group which is to be subscribed to.  Subscription groups are called in
            ascending order.
        @param fc
            String object containing the name of the script funtion that is to be
            subscribed to the Event.
        @return
            Connection object that can be used to check the status of the Event
            connection and to disconnect (unsubscribe) from the Event.
        */
        SignalPtr add(CommandObj * target, EventID id, Event::Group group, const String & fc);

        /** Subscribes the specified group of the named Event to a scripted funtion.
        @param target
            The target CommandObj for the subscription.
        @param name
            String object containing the name of the Event to subscribe to.
        @param group
            Group which is to be subscribed to.  Subscription groups are called in
            ascending order.
        @param functor
            String object containing the name of the script funtion that is to be
            subscribed to the Event.
        @param errorfunctor
            String containing the name of the script fuction that will be called
            if an error occurs while executing the handler function.  NB: This is
            the function passed as the error handler to lua_pcall.
        @return
            Connection object that can be used to check the status of the Event
            connection and to disconnect (unsubscribe) from the Event.
        */
        SignalPtr add(CommandObj * target, EventID id, Event::Group group, 
            const String & fc, const String & errorfc);

        /** Subscribes the specified group of the named Event to a scripted funtion.
        @param target
            The target CommandObj for the subscription.
        @param name
            String object containing the name of the Event to subscribe to.
        @param group
            Group which is to be subscribed to.  Subscription groups are called in
            ascending order.
        @param functor
            String object containing the name of the script funtion that is to be
            subscribed to the Event.
        @param error_handler
            integer value describing a lua registry reference to the function that
            will be called if an error occurs while executing the handler function.
            NB: This is the function passed as the error handler to lua_pcall.
        @return
            Connection object that can be used to check the status of the Event
            connection and to disconnect (unsubscribe) from the Event.
        */
        SignalPtr add(CommandObj * target, EventID id, Event::Group group,
            const String & fc, const int errorfc);

        /** Method called during system initialisation, prior to running any scripts
            via the ScriptModule, to enable the ScriptModule to perform any
            operations required to complete initialisation or binding of the script
            language to the gui system objects.
        @return Nothing.
        */
        void load(void);

        /** Method called during system destruction, after all scripts have been run
            via the ScriptModule, to enable the ScriptModule to perform any
            operations required to cleanup bindings of the script language to the
            gui system objects, as set-up in the earlier load call.
        @return Nothing.
        */
        void unload(void);

        /** Method used to get a pointer to the lua_State that the script module is
            attached to.
        @return
            A pointer to the lua_State that the script module is attached to.
        */
        lua_State * getLuaState(void) const {return d_state;}

        /** Set the name of the lua function that will be passed as the error
            handler in calls to lua_pcall, unless an alternative is specified
            in some other function call.
        @param error_handler_function
            Name of the lua function to be called.  This is looked up / bound on
            first usage.
        */
        void setDefaultPCallErrorHandler(const String & error_handler_function);

        /** Set the function that will be passed as the error handler in calls to
            lua_pcall, unless an alternative is specified in some other function
            call.
        @param function_reference
            Lua function registry index of the function to be called.
        */
        void setDefaultPCallErrorHandler(int function_reference);

        /** Return the function name string of the active error handler function.

            The 'active' error handler for the LuaScriptModule is either the
            default as specified by calling setDefaultPCallErrorHandler, or
            whatever might have been set up by a call to the internal
            initErrorHandlerFunc function.
        @note
            This function is really intended for use internally by other parts of
            the lua scripting support.  Although it could be useful elsewhere for
            advanced uses, so long as you're careful!
        */
        const String & getActivePCallErrorHandlerString() const;

        /** return the lua registry index of the active error handler function.
            The 'active' error handler for the LuaScriptModule is either the
            default as specified by calling setDefaultPCallErrorHandler, or
            whatever might have been set up by a call to the internal
            initErrorHandlerFunc function.
            @par This may return a value previously set by the user, or the value as
            bound internally by the script module for an error handler specified by
            function name.
        @note
            This function is really intended for use internally by other parts of
            the lua scripting support.  Although it could be useful elsewhere for
            advanced uses, so long as you're careful!
        \warning
            You should never call luaL_unref on the value returned by this function
            unless you created tge reference in the first place, and even then only
            after having called setDefaultPCallErrorHandler passing LUA_NOREF.
        */
        int getActivePCallErrorHandlerReference() const;
    private:
        /** Constructor for LuaScriptModule class which takes a lua_State
        @param state
            Pointer to the lua_State that the script module should attach to, if 0
            a new lua_State will be created.
        */
        LuaScriptModule(lua_State * state);

        /// Destructor for LuaScriptModule class.
        ~LuaScriptModule();


        void setModuleIdentifierString();
        /** Init the error handler function.  Return the lua stack index that
         *  should be passed to lua_pcall.  NB: This should be called prior to
         *  pushing any function parameters onto the lua stack.
         */
        int initErrorHandlerFunc();
        /** Init the error handler function.  Return the lua stack index that
         *  should be passed to lua_pcall.  NB: This should be called prior to
         *  pushing any function parameters onto the lua stack.
         */
        int initErrorHandlerFunc(const String func_name);
        /** Init the error handler function.  Return the lua stack index that
         *  should be passed to lua_pcall.  NB: This should be called prior to
         *  pushing any function parameters onto the lua stack.
         */
        int initErrorHandlerFunc(int func);

        /** do any needed cleanup after having called initErrorHandlerFunc and
         * (possible) executed a script.
         *
         * NB: This probably does less than you think ;)
         */
        void cleanupErrorHandlerFunc();

        /// release any reference we might have made to an error handling function.
        void unrefErrorFunc();

        /// Implementation function that executes a scipt file.
        void executeScriptFile_impl(const String & file, GroupID gid,
            const int err_idx, const int top);

        /// Implementation function that executes a global script function.
        int executeScriptGlobal_impl(const String & function_name,
            const int err_idx, const int top);

        /// Implementation function that executes a scripted event handler.
        bool executeScriptedEventHandler_impl(const String & handler_name,
            const EventArgs & e, const int err_idx, const int top);

        /// Implementation function that executes script contained in a String.
        void executeString_impl(const String & str, const int err_idx, const int top);
    protected:
        /// true when the attached lua_State was created by this script module
        bool d_ownsState;
        /// The lua_State that this script module uses.
        lua_State * d_state;
        /// Default error handler function to pass to lua_pcall.
        String d_errFuncName;
        /// Default registry index of the function to pass to lua_pcall.
        int d_errFuncIndex;
        /** Error handler function that is currently in use (as setup via a call
            to initErrorHandlerFunc)
        */
        String d_activeErrFuncName;
        /** Registry index of the function that is currently in use (as setup via a
            call to initErrorHandlerFunc)
        */
        int d_activeErrFuncIndex;
    };
}
#endif