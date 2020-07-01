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
#ifndef _NII_SINGLETON_H_
#define _NII_SINGLETON_H_

#include "NiiPreInclude.h"
#include "NiiException.h"

#if N_COMPILER == N_CPP_MSVC
#pragma warning (push)
 //stl警告
#pragma warning (disable : 4503)
#pragma warning (disable : 4251)
#pragma warning (disable : 4786)
#pragma warning (disable : 4503)
#pragma warning (disable : 4275)
#pragma warning(disable : 4311)
#pragma warning(disable : 4312)
#pragma warning(disable : 4661)

#pragma warning( disable: 4996)
#pragma warning (disable : 4100)
#pragma warning( disable : 4290)
#pragma warning( disable: 4661)

#pragma warning (disable : 201)
#endif

#if defined(NII_GCC_VISIBILITY)
    #pragma GCC visibility push(default)
#endif

namespace NII
{
    /// 模板类用于创建全局范围内的惟独实例.
    template <typename T> class Singleton
    {
    public:
        Singleton()
        {
#if defined(_MSC_VER) && _MSC_VER < 1200
            NIIi offset = (NIIi)(T*)1 - (NIIi)(Singleton <T>*)(T*)1;
            mOnly = (T*)((NIIi)this + offset);
#else
            mOnly = static_cast<T *>(this);
#endif
        }
        ~Singleton()
        {
            mOnly = 0;
        }
        /** 管理级别类专用
        @remark 一个应用进程中只有一个的类实例
        */
        static T & getOnly()
        {
            N_assert(mOnly, "error");
            return (*mOnly);
        }
        /** 管理级别类专用
        @remark 一个应用进程中只有一个的类实例
        */
        static T * getOnlyPtr()
        {
            return mOnly;
        }
    private:
        Singleton(const Singleton<T> &);
        Singleton & operator=(const Singleton<T> &);
    protected:
        static T * mOnly;
    };
}

#if defined (NII_GCC_VISIBILITY)
    #pragma GCC visibility pop
#endif

#if N_COMPILER == N_CPP_MSVC
    #pragma warning (pop)
#endif
#endif