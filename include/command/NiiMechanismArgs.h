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

#ifndef _NII_MECHANISM_ARGS_H_
#define _NII_MECHANISM_ARGS_H_

#include "NiiPreInclude.h"
#include "NiiEventArgs.h"
#include "NiiCommandObj.h"
#include "NiiEventObj.h"

namespace NII
{
namespace NII_COMMAND
{
    /// 对象事件列表结构
    class _EngineAPI MechanismArgs : public EventArgs
    {
    public:
        MechanismArgs(const CommandObj & co, const EventObj & eo);
        virtual ~MechanismArgs();

        const EventObj & mReceiver;     ///< 接收命令的对象
        const CommandObj & mSender;     ///< 接收命令的对象的目标操作对象
    };
}
}

#endif