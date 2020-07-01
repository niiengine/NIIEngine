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

#ifndef _NII_DELETE_COMMAND_
#define _NII_DELETE_COMMAND_

#include "NiiPreInclude.h"
#include "NiiCommand.h"
#include "NiiDefaultEventArgs.h"

namespace NII
{
namespace NII_COMMAND
{
    /** (可接受命令的对象)的创建命令
    @version NIIEngine 3.0.0
    */
    class _EngineAPI CreateCommand : public Command
    {
    public:
        CreateCommand();
        ~CreateCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** 当创建命令执行后的同时触发这个函数
        @version NIIEngine 3.0.0
        */
        void onCreated(const GenerateObjEventArgs & arg);
    private:
        GenerateObjEventArgs * mArg;
    };
    
    /** (可接受命令的对象)的删除命令
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DeleteCommand : public Command
    {
    public:
        DeleteCommand();
        ~DeleteCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** 当删除命令执行后的同时触发这个函数
        @version NIIEngine 3.0.0
        */
        void onDeleted(const DeleteObjEventArgs & arg);
    private:
        DeleteObjEventArgs * mArg;
    };
    
    /** (可接受命令的对象)的调整参数的命令
    @version NIIEngine 3.0.0
    */
    class _EngineAPI SetCommand : public Command
    {
    public:
        SetCommand();
        ~SetCommand();

        ///@copydetails Command::main
        void main();
    protected:
        /** 当调整参数命令执行后的同时触发这个函数
        @version NIIEngine 3.0.0
        */
        void onSet(const AdjustObjEventArgs & arg);
    protected:
        AdjustObjEventArgs * mArg;
    };
}
}

#endif