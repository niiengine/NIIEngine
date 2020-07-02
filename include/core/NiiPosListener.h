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

#ifndef _NII_PosListener_H_
#define _NII_PosListener_H_

#include "NiiPreProcess.h"

namespace NII
{
    /** 位置节监听
    @version NIIEngine 3.0.0
    */
    class _EngineAPI PosListener
    {
    public:
        PosListener();
        virtual ~PosListener();

        /** 创建时
        @version NIIEngine 3.0.0
        */
        virtual void create(PosNode * o);

        /** 删除时
        @version NIIEngine 3.0.0
        */
        virtual void destroy(PosNode * o);

        /** 添加子对象时
        @version NIIEngine 3.0.0
        */
        virtual void add(PosNode * obj, PosNode * child);

        /** 移去子对象时
        @version NIIEngine 3.0.0
        */
        virtual void remove(PosNode * o, PosNode * child);

        /** 附加到其他对象时
        @version NIIEngine 3.0.0
        */
        virtual void attach(PosNode * o);

        /** 从其他对象解除时
        @version NIIEngine 3.0.0
        */
        virtual void detach(PosNode * o);

        /** 更新时
        @version NIIEngine 3.0.0
        */
        virtual void update(PosNode * o);
    };
}
#endif