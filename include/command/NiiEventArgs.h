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

#ifndef _NII_EVENT_ARGS_H_
#define _NII_EVENT_ARGS_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 执行函子,类函数,方法函数,成员函数时需要的参数类
    @remark
        这个参数仅仅抽象为对象概念,不会改变一次性性质,为了效率很处理空间消耗等问题,需
        要尽可能使用const &等修饰成员变量
    @version NIIEngine 3.0.0
    */
    class _EngineAPI EventArgs : public EventAlloc
    {
    public:
        EventArgs();
        virtual ~EventArgs();
    };

    template <typename NT> class ValueEventArgs : public EventArgs
    {
    public:
        ValueEventArgs(const NT & value) : mValue(value) {}
        ~ValueEventArgs() {}

        const NT & mValue;
    };
}
#endif