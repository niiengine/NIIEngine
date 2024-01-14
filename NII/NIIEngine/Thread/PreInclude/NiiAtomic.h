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

#ifndef _NII_Atomic_H_
#define _NII_Atomic_H_

#include <signal.h>

#include "NiiPreInclude.h"
#include "NiiException.h"

namespace NII
{
    template <class T> class AtomicObject
    {
    public:
        AtomicObject(const T & initial) : mField(initial){}

        AtomicObject(const AtomicObject<T> & cousin) : mField(cousin.get()){}

        AtomicObject(){}

        void operator= (const AtomicObject<T> & cousin)
        {
            set(cousin.get());
        }

        T get() const
        {
            N_mutex1_lock
            return mField;
        }

        void set(const T & value)
        {
            N_mutex1_lock
            mField = value;
        }

        /** 效验状态,并设置新状态
        @remark 线程安全
        @param[in] olds 旧状态
        @param[in] news 新状态
        */
        bool cas(const T & olds, const T & news)
        {
            N_mutex1_lock

            if(mField != olds)
                return false;

            mField = news;
            return true;
        }

        T & operator++(void)
        {
            N_mutex1_lock
            return ++mField;
        }

        T & operator--(void)
        {
            N_mutex1_lock
            return --mField;
        }

        const T operator++(NIIi)
        {
            N_mutex1_lock
            return mField++;
        }

        const T operator--(NIIi)
        {
            N_mutex1_lock
            return mField--;
        }

        T & operator+=(const T &add)
        {
            N_mutex1_lock
            mField += add;
            return mField;
        }
    protected:
        N_mutex1
        volatile T mField;
    };
}

// These GCC instrinsics are not supported on ARM - masterfalcon
#if N_COMPILER == N_CPP_GNUC && NII_COMP_VER >= 412 && NII_THREAD_SUPPORT && NII_CPU != NII_CPU_ARM

namespace NII
{
    template<class T> class AtomicScalar
    {
    public:
        AtomicScalar(const T & initial) : mField(initial){}

        AtomicScalar(const AtomicScalar<T> & cousin) : mField(cousin.mField){}

        AtomicScalar(){}

        void operator= (const AtomicScalar<T> & cousin)
        {
            mField = cousin.mField;
        }

        T get () const
        {
            return mField;
        }

        void set (const T & value)
        {
            mField = value;
        }

        bool cas (const T & olds, const T & news)
        {
            return __sync_bool_compare_and_swap(&mField, olds, news);
        }

        T & operator++ (void)
        {
            __sync_add_and_fetch(&mField, 1);
        }

        T & operator-- (void)
        {
            __sync_add_and_fetch(&mField, -1);
        }

        const T operator++ (NIIi)
        {
            __sync_fetch_and_add(&mField, 1);
        }

        const T operator-- (NIIi)
        {
            __sync_fetch_and_add(&mField, -1);
        }

        T & operator+=(const T & add)
        {
            return __sync_add_and_fetch(&mField, add);
        }
        volatile T mField;
    };
}

#elif N_COMPILER == N_CPP_MSVC && NII_COMP_VER >= 1400 && NII_THREAD_SUPPORT
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #if !defined(NOMINMAX) && defined(_MSC_VER)
        #define NOMINMAX // required to stop windows.h messing up std::min
    #endif
    #include <windows.h>
    #include <intrin.h>
namespace NII
{
    template<class T> class AtomicScalar
    {
    public:
        AtomicScalar (const T & initial) : mField(initial){}

        AtomicScalar (const AtomicScalar<T> & cousin) : mField(cousin.mField){}

        AtomicScalar(){}

        void operator= (const AtomicScalar<T> & cousin)
        {
            mField = cousin.mField;
        }

        T get (void) const
        {
            return mField;
        }

        void set (const T & value)
        {
            mField = value;
        }

        bool cas (const T & olds, const T & news)
        {
            if (sizeof(T)==2)
            {
                return _InterlockedCompareExchange16((SHORT*)&mField, static_cast<SHORT>(news), static_cast<SHORT>(olds)) == static_cast<SHORT>(olds);
            }
            else if (sizeof(T)==4)
            {
                return _InterlockedCompareExchange((LONG*)&mField, static_cast<LONG>(news), static_cast<LONG>(olds)) == static_cast<LONG>(olds);
            }
            else if (sizeof(T)==8)
            {
                return _InterlockedCompareExchange64((LONGLONG*)&mField, static_cast<LONGLONG>(news), static_cast<LONGLONG>(olds)) == static_cast<LONGLONG>(olds);
            }
            else
            {
                N_EXCEPT(UnImpl,_I18n("Only 16, 32, and 64 bit scalars supported in win32."));
            }
        }

        T & operator++ (void)
        {
            if (sizeof(T)==2)
            {
                return InterlockedIncrement16((SHORT*)&mField);
            }
            else if (sizeof(T)==4)
            {
                return InterlockedIncrement((LONG*)&mField);
            }
            else if (sizeof(T)==8)
            {
                return InterlockedIncrement64((LONGLONG*)&mField);
            }
            else
            {
                N_EXCEPT(UnImpl,_I18n("Only 16, 32, and 64 bit scalars supported in win32."));
            }
        }

        T & operator-- (void)
        {
            if (sizeof(T)==2)
            {
                return InterlockedDecrement16((SHORT*)&mField);
            }
            else if (sizeof(T)==4)
            {
                return InterlockedDecrement((LONG*)&mField);
            }
            else if (sizeof(T)==8)
            {
                return InterlockedDecrement64((LONGLONG*)&mField);
            }
            else
            {
                N_EXCEPT(UnImpl,_I18n("Only 16, 32, and 64 bit scalars supported in win32."));
            }
        }

        const T operator++ (NIIi)
        {
            if (sizeof(T)==2)
            {
                return InterlockedIncrement16((SHORT*)&mField)-1;
            }
            else if (sizeof(T)==4)
            {
                return InterlockedIncrement((LONG*)&mField)-1;
            }
            else if (sizeof(T)==8)
            {
                return InterlockedIncrement64((LONGLONG*)&mField)-1;
            }
            else
            {
                N_EXCEPT(UnImpl,_I18n("Only 16, 32, and 64 bit scalars supported in win32."));
            }
        }

        const T operator-- (NIIi)
        {
            if (sizeof(T)==2)
            {
                return InterlockedDecrement16((SHORT*)&mField)+1;
            }
            else if (sizeof(T)==4)
            {
                return InterlockedDecrement((LONG*)&mField)+1;
            }
            else if (sizeof(T)==8)
            {
                return InterlockedDecrement64((LONGLONG*)&mField)+1;
            }
            else
            {
                N_EXCEPT(UnImpl,_I18n("Only 16, 32, and 64 bit scalars supported in win32."));
            }
        }
        T & operator+=(const T &add)
        {
            //The function InterlockedExchangeAdd is not available for 64 and 16 bit version
            //We will use the cas operation instead.
            T newVal;
            do
            {
                //Create a value of the current field plus the added value
                newVal = mField + add;
                //Replace the current field value with the new value. Ensure that the value
                //of the field hasn't changed in the mean time by comparing it to the new value
                //minus the added value.
            } while (!cas(newVal - add, newVal)); //repeat until successful
            return newVal;
        }
        volatile T mField;
    };
}
#else
namespace NII
{
    template <class T> class AtomicScalar
    {
    public:
        AtomicScalar(const T & initial) : mField(initial){}

        AtomicScalar(const AtomicScalar<T> & cousin) : mField(cousin.mField){}

        AtomicScalar(){}

        void operator=(const AtomicScalar<T> & cousin)
        {
            N_mutex1_lock
            mField = cousin.mField;
        }

        T get() const
        {
            // no lock required here
            // since get will not interfere with set or cas
            // we may get a stale value, but this is ok
            return mField;
        }

        void set(const T & value)
        {
            N_mutex1_lock
            mField = value;
        }

        bool cas(const T & olds, const T & news)
        {
            N_mutex1_lock
            if(mField != olds)
                return false;
            mField = news;
            return true;
        }

        T & operator++(void)
        {
            N_mutex1_lock
            return ++mField;
        }

        T & operator--(void)
        {
            N_mutex1_lock
            return --mField;
        }

        const T operator++(NIIi)
        {
            N_mutex1_lock
            return mField++;
        }

        const T operator--(NIIi)
        {
            N_mutex1_lock
            return mField--;
        }
        T & operator+=(const T & add)
        {
            N_mutex1_lock
            mField += add;
            return mField;
        }
    protected:
        N_mutex1
        volatile T mField;
    };
}
#endif //N_COMPILER == N_CPP_GNUC && NII_COMP_VER >= 412 && NII_THREAD_SUPPORT && NII_CPU != NII_CPU_ARM

#endif
