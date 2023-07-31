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

#ifndef _NII_SHAREPTR_H_
#define _NII_SHAREPTR_H_

#include "NiiPreInclude.h"
#include "NiiException.h"
#include "NiiThreadLock.h"

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
        virtual void equal(const SharedPtr<T> & o)
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
    #define Ptr(value) SharedPtr<value>
    
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
				ScopeLock templock(o.mMutex);
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
				ScopeLock templock(*o.mMutex);
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
            mMutex->lock();
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
                mMutex->unlock();
                delete mMutex;
                mMutex = 0;
                return;
            }
            mMutex->unlock();
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