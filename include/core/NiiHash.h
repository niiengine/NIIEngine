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

#ifndef _NII_HASH_H_
#define _NII_HASH_H_

#include "NiiPreInclude.h"

namespace NII
{
    /// 一般哈希快速算法
    Nui32 _EngineAPI FastHash(const Nui8 * data, NCount len, Nui32 ext = 0);

    /// 组合哈希
    template <typename T> Nui32 FastHash(Nui32 ext, const T & data)
    {
        return FastHash((const Nui8*)&data, sizeof(T), ext);
    }

    inline Nui32 FastHash(const String & src) 
    {
        return FastHash((const Nui8*)src.c_str(), sizeof(Ntchar) * src.length());
    }

    Nui32 _EngineAPI FastHash(FILE * fp);
    
    Nui64 _EngineAPI MurmurHash64A(const void * key, NCount size, Nui64 ext);
    Nui64 _EngineAPI MurmurHash64B(const void * key, NCount size, Nui64 ext);

    void _EngineAPI MurmurHash_x86_32(const void * key, NCount size, Nui32 ext, Nui32 & out);

    void _EngineAPI MurmurHash_x86_128(const void * key, NCount size, const Nui32 & ext[4], Nui32 & out[4]);

    void _EngineAPI MurmurHash_x64_128(const void * key, NCount size, const Nui64 & ext[2], Nui64 & out[2]);

    inline void MurmurHash_128(const void * key, NCount size, void * ext, void * out) 
    {
#if N_ARCH == N_ARCH_64
        MurmurHash_x64_128(key, size, *(const Nui32**)ext, *(Nui32**)out);
#else
        MurmurHash_x86_128(key, size, *(const Nui64**)ext, *(Nui64**)out);
#endif
    }
    
    /** 128bits hash
    */
    struct HashKey
    {
        bool operator < ( const HashKey & o ) const
        {
            if( mData[0] < o.mData[0] ) return true;
            if( mData[0] > o.mData[0] ) return false;

            if( mData[1] < o.mData[1] ) return true;

            return false;
        }
        
        Nui64 mData[2];
    };
    
    /// 哈希数组
    template <typename T> class HashedVector
    {
    public:
        typedef std::vector<T, StlAlloc<T, N_Alloc> > VectorImpl;
    public:
        typedef typename VectorImpl::value_type value_type;
        typedef typename VectorImpl::pointer pointer;
        typedef typename VectorImpl::reference reference;
        typedef typename VectorImpl::const_reference const_reference;
        typedef typename VectorImpl::size_type size_type;
        typedef typename VectorImpl::difference_type difference_type;
        typedef typename VectorImpl::iterator iterator;
        typedef typename VectorImpl::const_iterator const_iterator;
        typedef typename VectorImpl::reverse_iterator reverse_iterator;
        typedef typename VectorImpl::const_reverse_iterator const_reverse_iterator;

        HashedVector() : mListHash(0), mListHashDirty(false) {}
        HashedVector(size_type n) :
            mList(n),
            mListHash(0),
            mListHashDirty(n > 0) {}
        HashedVector(size_type n, const T & t) :
            mList(n, t),
            mListHash(0),
            mListHashDirty(n > 0) {}
        HashedVector(const HashedVector<T> & rhs) :
            mList(rhs.mList),
            mListHash(rhs.mListHash),
            mListHashDirty(rhs.mListHashDirty) {}

        template <class InputIterator> HashedVector(InputIterator a, InputIterator b)
            : mList(a, b), mListHashDirty(false)
        {
            dirtyHash();
        }

        ~HashedVector() {}

        void dirtyHash()
        {
            mListHashDirty = true;
        }

        bool isHashDirty() const
        {
            return mListHashDirty;
        }

        iterator begin()
        {
            // 假设哈希需要重新计算在非-const
            dirtyHash();
            return mList.begin();
        }
        iterator end() { return mList.end(); }
        const_iterator begin() const { return mList.begin(); }
        const_iterator end() const { return mList.end(); }
        reverse_iterator rbegin()
        {
            // 假设哈希需要重新计算在非-const
            dirtyHash();
            return mList.rbegin();
        }
        reverse_iterator rend() { return mList.rend(); }
        const_reverse_iterator rbegin() const { return mList.rbegin(); }
        const_reverse_iterator rend() const { return mList.rend(); }
        size_type size() const { return mList.size(); }
        size_type max_size() const { return mList.max_size(); }
        size_type capacity() const { return mList.capacity(); }
        bool empty() const { return mList.empty(); }
        reference operator[](size_type n)
        {
            // 假设哈希需要重新计算在非-const
            dirtyHash();
            return mList[n];
        }
        const_reference operator[](size_type n) const { return mList[n]; }
        reference at(size_type n)
        {
            // 假设哈希需要重新计算在非-const
            dirtyHash();
            return mList.const_iterator(n);
        }
        const_reference at(size_type n) const { return mList.at(n); }

        HashedVector<T> & operator=(const HashedVector<T>& rhs)
        {
            mList = rhs.mList;
            mListHash = rhs.mListHash;
            mListHashDirty = rhs.mListHashDirty;
            return *this;
        }

        void reserve(NCount t) { mList.reserve(t); }

        reference front()
        {
            // 假设哈希需要重新计算在非-const
            dirtyHash();
            return mList.front();
        }

        const_reference front() const { return mList.front(); }

        reference back()
        {
            // 假设哈希需要重新计算在非-const
            dirtyHash();
            return mList.back();
        }

        const_reference back() const { return mList.back(); }

        void push_back(const T & t)
        {
            mList.push_back(t);
            // 快速自由的哈希添加
            if (!isHashDirty())
                addToHash(t);
        }

        void pop_back()
        {
            mList.pop_back();
            dirtyHash();
        }

        void swap(HashedVector<T> & o)
        {
            mList.swap(o.mList);
            dirtyHash();
        }

        iterator insert(iterator pos, const T & t)
        {
            bool recalc = (pos != end());
            iterator ret = mList.insert(pos, t);
            if (recalc)
                dirtyHash();
            else
                addToHash(t);
            return ret;
        }

        template <class InputIterator> void insert(iterator pos, InputIterator f, InputIterator l)
        {
            mList.insert(pos, f, l);
            dirtyHash();
        }

        void insert(iterator pos, size_type n, const T & x)
        {
            mList.insert(pos, n, x);
            dirtyHash();
        }

        iterator erase(iterator pos)
        {
            iterator ret = mList.erase(pos);
            dirtyHash();
            return ret;
        }

        iterator erase(iterator first, iterator last)
        {
            iterator ret = mList.erase(first, last);
            dirtyHash();
            return ret;
        }

        void clear()
        {
            mList.clear();
            mListHash = 0;
            mListHashDirty = false;
        }

        void resize(size_type n, const T & t = T())
        {
            bool recalc = false;
            if (n != size())
                recalc = true;

            mList.resize(n, t);
            if (recalc)
                dirtyHash();
        }

        bool operator==(const HashedVector<T> & b)
        {
            return mListHash == b.mListHash;
        }

        bool operator<(const HashedVector<T> & b)
        {
            return mListHash < b.mListHash;
        }

        /// 获取哈希值
        Nui32 getHash() const
        {
            if (isHashDirty())
                recalcHash();

            return mListHash;
        }
    protected:
        void addToHash(const T & newPtr) const
        {
            mListHash = FastHash((const Nui8 *)&newPtr, sizeof(T), mListHash);
        }

        void recalcHash() const
        {
            mListHash = 0;
            for (const_iterator i = mList.begin(); i != mList.end(); ++i)
                addToHash(*i);
            mListHashDirty = false;
        }
    protected:
        VectorImpl mList;
        mutable Nui32 mListHash;
        mutable bool mListHashDirty;
    };
}

#endif