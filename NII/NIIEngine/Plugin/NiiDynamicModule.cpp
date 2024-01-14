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

#include "NiiPreProcess.h"
#include "NiiDynamicModule.h"

#include "NiiException.h"
#include "NiiLogManager.h"

#if N_PLAT == N_PLAT_WIN32
    #define WIN32_LEAN_AND_MEAN
    #if !defined(NOMINMAX) && defined(_MSC_VER)
        #define NOMINMAX // required to stop windows.h messing up std::min
    #endif
    #include <windows.h>
#endif

#if N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
    #include "macUtils.h"
    #include <dlfcn.h>
#endif

#if N_PLAT == N_PLAT_WIN32
    #define DYNLIB_LOAD(a) LoadLibraryEx(a, NULL, LOAD_WITH_ALTERED_SEARCH_PATH)
    #define DYNLIB_GETSYM(a, b) GetProcAddress(a, b)
    #define DYNLIB_UNLOAD(a) !FreeLibrary(a)
#elif N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_ANDROID
    #define DYNLIB_LOAD(a) dlopen(a, RTLD_LAZY | RTLD_GLOBAL)
    #define DYNLIB_GETSYM(a, b) dlsym(a, b)
    #define DYNLIB_UNLOAD(a) dlclose(a)
#elif N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
    #define DYNLIB_LOAD(a) mac_loadDylib(a)
    #define DYNLIB_GETSYM(a, b) dlsym(a, b)
    #define DYNLIB_UNLOAD(a) dlclose(a)
#endif

namespace NII
{
    //-----------------------------------------------------------------------
    DynamicModule::DynamicModule(const String & name)
    {
        mName = name;
        mInst = NULL;
    }
    //-----------------------------------------------------------------------
    DynamicModule::~DynamicModule()
    {
    }
    //-----------------------------------------------------------------------
    void DynamicModule::load()
    {
        // Log library load
        N_Only(Log).log(_I18n("Loading library ") + mName);

        String name = mName;
#if N_PLAT == N_PLAT_LINUX
        // dlopen() does not add .so to the filename, like windows does for .dll
        if (name.find(".so") == String::npos)
           name += ".so";
#elif N_PLAT == N_PLAT_OSX
        // dlopen() does not add .dylib to the filename, like windows does for .dll
        if (name.substr(name.length() - 6, 6) != ".dylib")
            name += ".dylib";
#elif N_PLAT == N_PLAT_WIN32
        // Although LoadLibraryEx will add .dll itself when you only specify the library name,
        // if you include a relative path then it does not. So, add it to be sure.
        if (name.substr(name.length() - 4, 4) != _T(".dll"))
            name += _T(".dll");
#endif
        if(!DYNLIB_PATH.empty())
            mInst = (DYNLIB_HANDLE)DYNLIB_LOAD((WCHAR*)(DYNLIB_PATH + name).c_str());
        if(!mInst)
            mInst = (DYNLIB_HANDLE)DYNLIB_LOAD((WCHAR*)name.c_str());
        if(!mInst)
            N_EXCEPT(Internal, _I18n("Could not load dynamic library ") + 
                mName + _I18n(".  System Error: ") + getLastError());
    }
    //-----------------------------------------------------------------------
    void DynamicModule::unload()
    {
        // Log library unload
        N_Only(Log).log(_I18n("Unloading library ") + mName);

        if(DYNLIB_UNLOAD(mInst))
        {
            N_EXCEPT(Internal, _I18n("Could not unload dynamic library ") + 
                mName + _I18n(".  System Error: ") + getLastError());
        }
    }
    //-----------------------------------------------------------------------
    const String & DynamicModule::getName() const 
    { 
        return mName; 
    }
    //-----------------------------------------------------------------------
    void * DynamicModule::getSymbol(const VString & name) const throw()
    {
        return (void*)DYNLIB_GETSYM(mInst, name.c_str());
    }
    //-----------------------------------------------------------------------
    String DynamicModule::getLastError() 
    {
#if N_PLAT == N_PLAT_WIN32
        LPVOID lpMsgBuf; 
        FormatMessage( 
            FORMAT_MESSAGE_ALLOCATE_BUFFER | 
            FORMAT_MESSAGE_FROM_SYSTEM | 
            FORMAT_MESSAGE_IGNORE_INSERTS, 
            NULL, 
            GetLastError(), 
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
            (LPTSTR) &lpMsgBuf, 
            0, 
            NULL 
            ); 
        String ret = (Ntchar*)lpMsgBuf;
        // Free the buffer.
        LocalFree(lpMsgBuf);
        return ret;
#elif N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_OSX
        return String(dlerror());
#else
        return String("");
#endif
    }
    //-----------------------------------------------------------------------
}