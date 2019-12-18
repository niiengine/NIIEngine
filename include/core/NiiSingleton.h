/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/
/* Original version Copyright (C) Scott Bilas, 2000.
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Scott Bilas, 2000"
 */
#ifndef _NII_SINGLETON_H_
#define _NII_SINGLETON_H_

#include "NiiPreInclude.h"
#include "NiiHeaderPrefix.h"
#include "NiiException.h"

#if N_COMPILER == N_CPP_MSVC
    #pragma warning(disable : 4311)
    #pragma warning(disable : 4312)
	#pragma warning(disable : 4661) 
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
        @remark 一个应用进程中只有唯一一个的类实例
        */
        static T & getOnly()
        {
            N_assert(mOnly, "error");
            return (*mOnly);
        }
        /** 管理级别类专用
        @remark 一个应用进程中只有唯一一个的类实例
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

#include "NiiHeaderSuffix.h"
#endif