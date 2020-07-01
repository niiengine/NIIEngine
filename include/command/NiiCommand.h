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

#ifndef _NII_COMMAND_H_
#define _NII_COMMAND_H_

#include "NiiPreInclude.h"
#include "NiiEvent.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 命令
    @remark
        函子槽处理集,可以包含多个函子,处理函子可以用于传播执行命令后的影响,由于具
        体命令是属于具体对象,如果想需要传播信息给非彼对象的其他对象可以通过添加处
        理函子操作
    @par 这个类像Event类
    @vesion NIIEngine 3.0.0
    */
    class _EngineAPI Command : public EventAlloc
    {
    public:
        typedef vector<EventFunctor *>::type Funcs;
    public:
        Command();
        virtual ~Command();

        /** 添加一个执行函子,不会检测重复添加问题
        @remark 在命令执行的时候将会同时执行这些函子
        @param[in] func 需要执行的函子槽
        @version NIIEngine 3.0.0
        */
        void add(EventFunctor * func);

        /** 移去一个执行函子,不会检测重复添加问题
        @remark
        @param[in] func 需要删除的函子槽
        @version NIIEngine 3.0.0
        */
        void remove(EventFunctor * func);

        /** 执行这个对象(命令)
        @remark 在命令执行的时候将会同时执行这些函子
        @version NIIEngine 3.0.0
        */
        void operator()(const EventArgs * args);

        /** 命令的核心
        @remark 需要执行的主要部件
        @version NIIEngine 3.0.0
        */
        virtual void main();
    protected:
        /** 当命令执行后的同时触发这个函数
        @version NIIEngine 3.0.0 高级api
        */
        virtual void onDone(const EventArgs * arg);
    protected:
        Funcs mExecutes;    ///< 需要执行的衍生函子列表
    };
}
}
#endif