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

#ifndef _NII_NET_OBJ_H_
#define _NII_NET_OBJ_H_

#include "NiiNetPreInclude.h"
#include "NiiNetSerializer.h"

namespace NII
{
    /** ÍøÂç¶ÔÏó
    @version NIIEngine 4.0.0
    */
    class _EngineAPI NetObj
    {
    public:
        NetObj();
        virtual ~NetObj();

        virtual void write(const NetSerializer * out);
        virtual void read(NetSerializer * in) const;
    };
}
#endif