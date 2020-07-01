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

#ifndef _NII_DEFAULT_CONDITION_H_
#define _NII_DEFAULT_CONDITION_H_

#include "NiiPreInclude.h"
#include "NiiCondition.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 条件判断器的默认实现
    @remark 默认的实现是,假设条件的所有参子都达成,则触发调用命令,为了实际产生的效率,
        系统限制一个条件仅仅能有32组判断参子
    @version NIIEngine 3.0.0 内部类
    */
    class _EngineAPI DefaultCondition : public Condition
    {
    public:
		DefaultCondition();
        DefaultCondition(Command * command);
        ~DefaultCondition();

        ///@copydetails Condition::reset
        void compile();
    protected:
        ///@copydetails Condition::reach
        void reach(const ConditionParam * cp);

        ///@copydetails Condition::reach
        void reach();

        ///@copydetails Condition::fail
        void fail(const ConditionParam * cp);

        ///@copydetails Condition::fail
        void fail();

        ///@copydetails Condition::reached
        bool reached();
    private:
        Nui32 mComparesMark;
    };
}
}
#endif