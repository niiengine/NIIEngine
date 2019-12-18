/*
-----------------------------------------------------------------------------
大型多媒体框架

时间: 2015-5-7

文本编码: utf-8

所属公司: 深圳闽登科技有限公司

命名风格: 概论命名法

编程风格: 统筹式

管理模式: 分布式

内部成分: UI对象 网络对象 音频对象 物理对象 事件驱动对象(扩散性设计)

主要成分: c++(80%) c(20%)

用途: 操作系统桌面(包围操作系统内核api)
      三维应用软件
        计算机辅助立体设计软件(CAD)
        地理信息系统软件(GIS)
        电影背景立体重构软件
        立体游戏软件

偏向用途: 立体游戏软件

主页: www.niiengine.com 电子邮箱: niiengine@gmail.com OR niiengine@163.com

授权方式:商业授权(www.niiengine.com/license)(3种)
------------------------------------------------------------------------------
*/

#ifndef _NII_HASH_H_
#define _NII_HASH_H_

#include "NiiPreInclude.h"

namespace NII
{
    /// 一般哈希快速算法
    Nui32 _EngineAPI FastHash(const Nui8 * data, NCount len, Nui32 ext = 0);

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