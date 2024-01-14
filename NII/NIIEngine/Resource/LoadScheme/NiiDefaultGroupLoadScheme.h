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

#ifndef _NII_DEFAULT_GROUP_LOAD_SCHEME_H_
#define _NII_DEFAULT_GROUP_LOAD_SCHEME_H_

#include "NiiPreInclude.h"
#include "NiiGroupLoadScheme.h"

namespace NII
{
    /** 默认的资源组加载策略
    @remark 不使用多线程协调加载,哪个线程调用了request哪个线程就负责加载资源组
    @
    */
    class _EngineInner DefaultGroupLoadScheme : public GroupLoadScheme
    {
    public:
        DefaultGroupLoadScheme(SchemeID sid, GroupID gid, NCount wl);

        /// @copydetails LoadScheme::request
        bool request(LoadingState ls);
    };
}
#endif