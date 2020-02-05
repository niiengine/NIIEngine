/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2014-7-11

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

#ifndef _NII_SHAREPTR_H_
#define _NII_SHAREPTR_H_

#include "NiiPreInclude.h"
#include "NiiException.h"

namespace NII
{
    /** 共享对象
    @note
        如果能确定被包装的对象仅仅在一个线程中调用则使用这个类,否则使用SharedPtrST
    @version NIIEngine 3.0.0
    */
    template<class T> class SharedPtr
    {
    public:
        /// 析构中用来释放内存的方法
        enum MemMode
        {
            MU_Delete,      ///< 使用N_delete
            MU_Delete_T,    ///< 使用N_delete_t
            MU_Free         ///< 使用N_free
        };
    public:
        SharedPtr() :
            mPtr(0),
            mCount(0),
            mMode(MU_Delete){}

        explicit SharedPtr(T * obj, MemMode mode = MU_Delete) :
            mPtr(obj),
            mCount(obj ? (NCount *)N_alloc(sizeof(NCount)) : 0),
            mMode(mode)
        {
            if (mCount)
            {
                *mCount = 1;
            }
        }

        SharedPtr(const SharedPtr & o) :
            mPtr(o.mPtr),
            mCount(o.mCount),
            mMode(o.mMode)
        {
            if(mCount)
            {
                ++(*mCount);
            }
        }

        template<class Y> SharedPtr(const SharedPtr<Y> & o) :
            mPtr(o.mPtr),
            mCount(o.mCount),
            mMode(o.mMode)
        {
            if(mCount)
            {
                ++(*mCount);
            }
        }

        virtual ~SharedPtr()
        {
            setNull();
        }

        SharedPtr & operator=(const SharedPtr & o)
        {
            if(mPtr != o.mPtr)
            {
                if(mPtr)
                {
                    release();
                }
                if(o.mPtr)
                {
                    mCount = o.mCount;
                    ++(*mCount);
                    mPtr = o.mPtr;
                    mMode = o.mMode;
                    equal(o);
                }
                else
                {
                    mCount = 0;
                }
            }
            return *this;
        }

        template<class Y> SharedPtr & operator=(const SharedPtr<Y> & o)
        {
            if(mPtr != o.mPtr)
            {
                if(mPtr)
                {
                    release();
                }
                if(o.mPtr)
                {
                    mCount = o.mCount;
                    ++(*mCount);
                    mPtr = o.mPtr;
                    mMode = o.mMode;
                    equal(o);
                }
                else
                {
                    mCount = 0;
                }
            }
            return *this;
        }

        inline void setNull()
        {
            if(mPtr)
            {
                release();
            }
            else
            {
                mCount = 0;                
            }
        }

        inline bool isNull() const
        {
            return mPtr == 0;
        }

        inline bool operator!() const
        {
            return mPtr == 0;
        }

        inline T & operator*() const
        {
            N_assert(mPtr, "error");
            return *mPtr;
        }

        inline T * operator->() const
        {
            N_assert(mPtr, "error");
            return mPtr;
        }

        inline T * get() const
        {
            return mPtr;
        }

        inline MemMode getMemMode() const
        {
            return mMode;
        }

        void bind(T * obj, MemMode mode = MU_Delete)
        {
            setNull();
            mCount = N_alloc_t(NCount, 1);
            *mCount = 1;
            mPtr = obj;
            mMode = mode;
        }

        inline bool isValid() const
        {
            return mPtr != 0;
        }

        inline bool isOneRef() const
        {
            N_assert(mCount, "error");
            return *mCount == 1;
        }

        inline NCount getRefCount() const
        {
            N_assert(mCount, "error");
            return *mCount;
        }

        inline NCount * getRefCountPtr() const
        {
            return mCount;
        }

        inline T * getPointer() const
        {
            return mPtr;
        }
    protected:
        virtual void equal(SharedPtr<T> & o)
        {
        }

        void release()
        {
            if(--(*mCount) == 0)
            {
                switch(mMode)
                {
                case MU_Delete:
                    N_delete mPtr;
                    break;
                case MU_Delete_T:
                    N_delete_t(mPtr, T);
                    break;
                case MU_Free:
                    N_free(mPtr);
                    break;
                };
                N_free(mCount);
                mPtr = 0;
                mCount = 0;
            }
        }
    protected:
        T * mPtr;
        NCount * mCount;
        MemMode mMode;
    };

    template<class T, class U> inline bool operator == (SharedPtr<T> const & l, SharedPtr<U> const & r)
    {
        return l.get() == r.get();
    }

    template<class T, class U> inline bool operator != (SharedPtr<T> const & l, SharedPtr<U> const & r)
    {
        return l.get() != r.get();
    }
    #define Ptr(value) SharedPtrST<value>
}
#endif