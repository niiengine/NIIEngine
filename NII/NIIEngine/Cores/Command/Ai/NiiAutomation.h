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

#ifndef _NII_AUTOMATION_H_
#define _NII_AUTOMATION_H_

#include "NiiPreInclude.h"

namespace NII
{
namespace NII_COMMAND
{
    /** 注解 这个类存储自动化类
    */
    class Automation
    {
    public:
        //比较操作数据
        enum
        {
            Equal,
            Unequal,
            Great,
            Little,
            Ungreat,
            Unlittle
        }
        //驱动关联事件关系联
        enum Relation
        {
            Position,
            Binding,
            //anymore
        }
        void CreateGenerateEvent();
        void Condition();
        void Apply();
        void 
    protected:
        
    };
}
}
#endif //