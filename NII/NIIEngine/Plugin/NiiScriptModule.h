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

#ifndef _NII_ScriptModule_H_
#define _NII_ScriptModule_H_

#include "NiiPreInclude.h"
#include "NiiCommandObj.h"

namespace NII
{
    /** 脚本模块
    @version NIIEngine 3.0.0
    */
    class _EngineAPI ScriptModule : public UIAlloc
    {
    public:
        ScriptModule();
        virtual ~ScriptModule();

        /** 加载脚本模块
        @version NIIEngine 3.0.0
        */
        virtual void load();

        /** 卸载脚本模块
        @version NIIEngine 3.0.0
        */
        virtual void unload();

        /** 脚本模块ID
        @version NIIEngine 3.0.0
        */
        ScriptModuleID getID() const;

        /** 执行脚本文件
        @version NIIEngine 3.0.0
        */
        virtual void executeFile(const String & file, GroupID gid = 0) = 0;

        /** 执行脚本函数
        @version NIIEngine 3.0.0
        */
        virtual int executeFunc(const String & func) = 0;

        /** 执行脚本函数
        @param func 函数
        @param arg 参数
        @version NIIEngine 3.0.0
        */
        virtual bool executeFunc(const String & func, const EventArgs & arg) = 0;

        /** 执行脚本
        @version NIIEngine 3.0.0
        */
        virtual void executeScript(const String & script) = 0;

        /** 捆绑脚本函数到命令对象中
        @param obj 命令对象
        @param eid 事件ID
        @param func 脚本函数
        @version NIIEngine 3.0.0
        */
        virtual SignalPtr add(NII_COMMAND::CommandObj * obj, EventID eid, const String & func) = 0;

        /** 脚本模块
        @version NIIEngine 3.0.0
        */
        static void setGroup(GroupID gid);

        /** 脚本模块资源组
        @version NIIEngine 3.0.0
        */
        static GroupID getGroup();
    protected:
        ScriptModuleID mID;
        static GroupID mResourceGroup;
    };

    class _EngineAPI ScriptFunctor : public SysAlloc
    {
    public:
        ScriptFunctor(ScriptModuleID mid, const String & fc) :mModuleID(mid), mFuncName(fc) {}
        ScriptFunctor(const ScriptFunctor & o) : mFuncName(o.mFuncName) {}
        bool operator()(const EventArgs & e) const;
    private:
        ScriptFunctor & operator=(const ScriptFunctor & o);

        String mFuncName;
        ScriptModuleID mModuleID;
    };
}
#endif