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

#ifndef _NII_RESOURCE_LOAD_SCHEME_H_
#define _NII_RESOURCE_LOAD_SCHEME_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 资源加载策略
    @remark 更局限于具体资源的加载过程
    @note 主动策略
    */
    class _EngineAPI ResLoadScheme : public ResourceAlloc
    {
    public:
        ResLoadScheme();
        virtual ~ResLoadScheme();

        /** 当想加载资源时调用它,注意这有可能会在后台线程调用,或者是半线程
            (NII_THREAD_SUPPORT == 2)因此,您不能从访问rendersystem回调这个.
            不要用loadResource加载东西
        @param[in] resource 希望加载的资源
        */
        virtual void plan(Resource * resource) { (void)resource; }

        /** 当想准备资源时调用它.
        @param[in] resource 希望准备的资源
        */
        virtual void load(Resource * resource) {};
    };
}
#endif