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
            MU_Delete,    ///< 使用N_delete
            MU_Delete_T,  ///< 使用N_delete_t
            MU_Free       ///< 使用N_free
        };
    public:
        NII_AUTO_SHARED_MUTEX

        /** 基本函数
        @version NIIEngine 3.0.0
        */
        SharedPtrST() :
            mPtr(0),
            mCount(0),
            mMode(MU_Delete)
        {
            NII_SET_AUTO_SHARED_MUTEX_NULL
        }

        /** 构造函数
        @param[in] ptr 指针实体
        @param[in] usage 用来释放内存的机制
        */
        template<class Y> explicit SharedPtrST(Y * ptr, MemMode usage = MU_Delete) :
            mPtr(ptr),
            mCount(ptr ? N_new_t(NCount)(1) : 0),
            mMode(usage)
        {
            NII_SET_AUTO_SHARED_MUTEX_NULL
            if(ptr)
            {
                NII_NEW_AUTO_SHARED_MUTEX
            }
        }

        SharedPtrST(const SharedPtrST & r) :
            mPtr(0),
            mCount(0),
            mMode(MU_Delete)
        {
            NII_SET_AUTO_SHARED_MUTEX_NULL
            NII_MUTEX_CONDITIONAL(r.N_mutex1_name)
            {
                N_lock_mutex(*r.N_mutex1_name)
                NII_COPY_AUTO_SHARED_MUTEX(r.N_mutex1_name)
                mPtr = r.mPtr;
                mCount = r.mCount;
                mMode = r.mMode;
                if(mCount)
                {
                    ++(*mCount);
                }
            }
        }

        template< class Y> SharedPtrST(const SharedPtrST<Y> & r) :
            mPtr(0),
            mCount(0),
            mMode(MU_Delete)
        {
            NII_SET_AUTO_SHARED_MUTEX_NULL
            NII_MUTEX_CONDITIONAL(r.N_mutex1_name)
            {
                N_lock_mutex(*r.N_mutex1_name)
                NII_COPY_AUTO_SHARED_MUTEX(r.N_mutex1_name)
                mPtr = r.getPointer();
                mCount = r.getRefCountPtr();
                mMode = r.getMemMode();
                if(mCount)
                {
                    ++(*mCount);
                }
            }
        }

        virtual ~SharedPtrST()
        {
            release();
        }

        SharedPtrST & operator=(const SharedPtrST & r)
        {
            if (mPtr == r.mPtr)
                return *this;
            SharedPtrST<T> tmp(r);
            swap(tmp);
            return *this;
        }

        template<class Y> SharedPtrST & operator=(const SharedPtrST<Y> & r)
        {
            if(mPtr == r.getPointer())
                return *this;
            SharedPtrST<T> tmp(r);
            swap(tmp);
            return *this;
        }

        inline void setNull()
        {
            if (mPtr)
            {
                release();
                mPtr = 0; //todo
                mCount = 0; //todo
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

        /** 绑定 rep 到 SharedPtrST.
        @remark 假设 SharedPtrST 使用未初始化的内存!
        */
        void bind(T * rep, MemMode usage = MU_Delete)
        {
            N_assert(!mPtr && !mCount, "error");
            NII_NEW_AUTO_SHARED_MUTEX
            NII_LOCK_AUTO_SHARED_MUTEX
            mCount = N_new_t(NCount)(1);
            mPtr = rep;
            mMode = usage;
        }

        inline bool isOneRef() const
        {
            NII_LOCK_AUTO_SHARED_MUTEX
            N_assert(mCount, "error");
            return *mCount == 1;
        }

        inline NCount getRefCount() const
        {
            NII_LOCK_AUTO_SHARED_MUTEX
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
        inline void release()
        {
            bool destroyThis = false;

            // 如果互斥锁未初始化化为一个非0值,即不是pUseCount也不是pRep
            NII_MUTEX_CONDITIONAL(N_mutex1_name)
            {
                // 有限作用域锁定自己的互斥(必须在删除前解锁)
                NII_LOCK_AUTO_SHARED_MUTEX
                if (mCount)
                {
                    if (--(*mCount) == 0)
                    {
                        destroyThis = true;
                    }
                }
            }
            if (destroyThis)
                destroy();

            NII_SET_AUTO_SHARED_MUTEX_NULL
        }

        virtual void destroy()
        {
            /* 如果你在这里获取是一个崩溃.你忘了在关闭NII前释放指针.在关闭前
                使用setNull(),或确保你的指针离开了范围在NII关闭前避免它.
            */
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
            NII_DELETE_AUTO_SHARED_MUTEX
        }

        virtual void swap(SharedPtrST<T> & o)
        {
            std::swap(mPtr, o.mPtr);
            std::swap(mCount, o.mCount);
            std::swap(mMode, o.mMode);
            //使用了线程
#if NII_THREAD_SUPPORT
            std::swap(N_mutex1_name, o.N_mutex1_name);
#endif
        }
    protected:
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
    /** @} */
    /** @} */
}
#endif