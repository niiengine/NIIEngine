/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2016-1-7

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

#ifndef _NII_SHAREDPTRST_H_
#define _NII_SHAREDPTRST_H_

#include "NiiPreInclude.h"
#include "NiiException.h"

namespace NII
{
    /** 共享对象(线程安全/多线程对象专用)
    @note 
        如果能确定被包装的对象是会2个不同的线程调用则需要使用这个类,否则使用SharedPtr
    @versin NIIEngine 3.0.0
    */
    template<class T> class SharedPtrST
    {
    public:
        enum MemMode
        {
            MU_Delete,      ///< 使用N_delete
            MU_Delete_T,    ///< 使用N_delete_t
            MU_Free         ///< 使用N_free
        };
    public:
        SharedPtrST() :
            mPtr(0),
            mCount(0),
            mMode(MU_Delete)
        {
            mMutex = 0;
        }

        explicit SharedPtrST(T * ptr, MemMode usage = MU_Delete) :
            mPtr(ptr),
            mMode(usage)
        {
            if(ptr)
            {
                mMutex = new ThreadMutex();
                mCount = N_new_t(NCount)(1);
            }
            else
            {
                mMutex = 0;
                mCount = 0;
            }
        }

        SharedPtrST(const SharedPtrST & o)
        {
            if(o.mMutex)
            {
                ScopeLock templock(*o.mMutex)
                ++(*o.mCount);
                mMutex = o.mMutex;
                mPtr = o.mPtr;
                mCount = o.mCount;
                mMode = o.mMode;
            }
            else
            {
                mMutex = 0; 
                mCount = 0;
                mPtr = 0;
                mMode = MU_Delete;
            }
        }

        template<class Y> SharedPtrST(const SharedPtrST<Y> & o)
        {
            if(o.mMutex)
            {
                ScopeLock templock(*o.mMutex)
                ++(*o.mCount);
                mMutex = o.mMutex;
                mPtr = o.mPtr;
                mCount = o.mCount;
                mMode = o.mMode;
            }
            else
            {
                mMutex = 0;
                mCount = 0;
                mPtr = 0;
                mMode = MU_Delete;
            }
        }

        virtual ~SharedPtrST()
        {
            setNull();
        }

        SharedPtrST & operator=(const SharedPtrST & o)
        {
            if(mPtr != o.mPtr)
            {
                if(mPtr)
                {
                    release();
                }
                if(o.mMutex)
                {
                    ScopeLock templock(o.mMutex);
                    if(o.mPtr)
                    {
                        ++(*o.mCount);
                        mCount = o.mCount;
                        mPtr = o.mPtr;
                        mMode = o.mMode;
                        mMutex = o.mMutex;
                        equal(o);
                    }
                    else
                    {
                        mCount = 0;
                        mMutex = 0;
                    }
                }
            }
            return *this;
        }

        template<class Y> SharedPtrST & operator=(const SharedPtrST<Y> & o)
        {
            if(mPtr != o.mPtr)
            {
                if(mPtr)
                {
                    release();
                }
                if(o.mMutex)
                {
                    ScopeLock templock(o.mMutex);
                    if(o.mPtr)
                    {
                        ++(*o.mCount);
                        mCount = o.mCount;
                        mPtr = o.mPtr;
                        mMode = o.mMode;
                        mMutex = o.mMutex;
                        equal(o);
                    }
                    else
                    {
                        mCount = 0;
                        mMutex = 0;
                    }
                }
            }
            return *this;
        }

        inline void setNull()
        {
            if (mPtr)
            {
                release();
            }
            else
            {
                mCount = 0;
                mMutex = 0;
            }
        }

        inline bool isNull() const
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

        void bind(T * rep, MemMode usage = MU_Delete)
        {
            setNull(); 
            mMutex = new ThreadMutex();
            ScopeLock tmplock(mMutex);           
            mCount = N_new_t(NCount)(1);
            mPtr = rep;
            mMode = usage;
        }

        inline bool isOneRef() const
        {
            ScopeLock tmplock(mMutex);
            N_assert(mCount, "error");
            return *mCount == 1;
        }

        inline NCount getRefCount() const
        {
            ScopeLock tmplock(mMutex);
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
        virtual void equal(SharedPtr<T> & o){}
        
        void release()
        {
            mMutex.lock();
            if (--(*mCount) == 0)
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
                mCount = 0;
                mPtr = 0;
                mMutex.unlock();
                delete mMutex;
                mMutex = 0;
                return;
            }
            mMutex.unlock();
        }
    protected:
        ThreadMutex * mMutex;
        T * mPtr;
        NCount * mCount;
        MemMode mMode;
    };

    template<class T, class U>
        inline bool operator == (SharedPtrST<T> const & l, SharedPtrST<U> const & r)
    {
        return l.get() == r.get();
    }

    template<class T, class U>
        inline bool operator != (SharedPtrST<T> const & l, SharedPtrST<U> const & r)
    {
        return l.get() != r.get();
    }

    template<class T, class U>
        inline bool operator < (SharedPtrST<T> const & l, SharedPtrST<U> const & r)
    {
        return std::less<const void *>()(l.get(), r.get());
    }

    #define STPtr(value) SharedPtrST<value>
}
#endif