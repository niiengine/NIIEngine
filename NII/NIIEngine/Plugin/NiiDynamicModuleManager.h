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

#ifndef _NII_DynamicModuleManager_H_
#define _NII_DynamicModuleManager_H_

#include "NiiPreInclude.h"
#include "NiiSingleton.h"
#include "NiiDynamicModule.h"

namespace NII
{
    /** 动态加载库的管理者
    @verison NIIEngine 3.0.0
    */
    class _EngineAPI DynamicModuleManager: public Singleton<DynamicModuleManager>, public DynamicModuleAlloc
    {
        friend class Engine;
    public:
        typedef map<String, DynamicModule *>::type DynList;
    public:
        /** 加载库
        @param[in] file 文件名字
        @version NIIEngine 3.0.0
        */
        DynamicModule * load(const String & file);

        /** 卸载库
        @param[in] file 文件名字
        @version NIIEngine 3.0.0
        */
        void unload(DynamicModule * lib);
    protected:
        DynamicModuleManager();
        ~DynamicModuleManager();
    protected:
        DynList mLibList;
    };
}

#endif