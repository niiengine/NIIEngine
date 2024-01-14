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
#ifndef _NII_DynamicModule_H_
#define _NII_DynamicModule_H_

#include "NiiPreInclude.h"

#if N_PLAT == N_PLAT_WIN32
#define DYNLIB_HANDLE hInstance
#define DYNLIB_EXT ".dll"
#define DYNLIB_PATH String(_T("../bin/"))
struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;
#elif N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_ANDROID
#define DYNLIB_HANDLE void *
#define DYNLIB_EXT ".so"
#elif N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
#define DYNLIB_HANDLE void*
#define DYNLIB_EXT ".dylib"
#define DYNLIB_PATH String("@executable_path/../Frameworks/")
#endif 

namespace NII
{
    /** 动态库
    @version NIIEngine 3.0.0
    */
    class _EngineAPI DynamicModule : public DynamicModuleAlloc
    {
    public:
        DynamicModule(const String & name);
        ~DynamicModule();

        /** 加载库
        @version NIIEngine 3.0.0
        */
        void load();

        /** 卸载库
        @version NIIEngine 3.0.0
        */
        void unload();

        /** 获取库的名字
        @version NIIEngine 3.0.0
        */
        const String & getName() const;

        /** 返回符号地址
        @param[in] name 查找符号的名字
        @return 如果这个函数成功.返回符号的句柄值,否则返回0
        @version NIIEngine 3.0.0
        */
        void * getSymbol(const VString & name) const throw();
    protected:
        /** 获取最后加载错误
        @version NIIEngine 3.0.0
        */
        String getLastError();
    protected:
        String mName;
        DYNLIB_HANDLE mInst;
    };
}
#endif