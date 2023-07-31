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

#ifndef _NII_StdHeaders_H_
#define _NII_StdHeaders_H_

#ifdef __BORLANDC__
    #define __STD_ALGORITHM
#endif

#if defined (NII_GCC_VISIBILITY)
    #pragma GCC visibility push(default)
#endif

#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <cstdarg>
#include <ctime>
#include <cstring>
#include <cassert>

#include <string>
#include <vector>
#include <map>
#include <list>
#include <set>
#include <deque>
#include <queue>
#include <bitset>

#if (N_COMPILER == N_CPP_GNUC) && !defined(STLPORT)
    #if N_COMPILER_VER >= 430
        #include <tr1/unordered_map>
        #include <tr1/unordered_set>
    #else
        #include <ext/hash_map>
        #include <ext/hash_set>
    #endif
#elif (N_COMPILER == N_CPP_GCCE)
    #if defined(_LIBCPP_VERSION)
        #include <unordered_map>
        #include <unordered_set>
    #else
        #include <tr1/unordered_map>
        #include <tr1/unordered_set>
    #endif
#else
    #if (N_COMPILER == N_CPP_MSVC) && !defined(STLPORT) && N_COMPILER_VER >= 1600 // VC++ 10.0
        #include <unordered_map>
        #include <unordered_set>
    #else
        #include <hash_set>
        #include <hash_map>
    #endif
#endif

#include <algorithm>
#include <functional>
#include <limits>

#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

#if N_COMPILER == N_CPP_GNUC && N_COMPILER_VER >= 310 && !defined(STLPORT)

#if N_COMPILER_VER >= 430
    #if __cplusplus >= 201103L
        #define HashMap         ::std::unordered_map
        #define HashSet         ::std::unordered_set
        #define HashMultiMap    ::std::unordered_multimap
        #define HashMultiSet    ::std::unordered_multiset
    #else
        #define HashMap         ::std::tr1::unordered_map
        #define HashSet         ::std::tr1::unordered_set
        #define HashMultiMap    ::std::tr1::unordered_multimap
        #define HashMultiSet    ::std::tr1::unordered_multiset
    #endif
    #define HashSpace           ::std
#else
    #define HashMap             ::__gnu_cxx::hash_map
    #define HashSet             ::__gnu_cxx::hash_set
    #define HashMultiMap        ::__gnu_cxx::hash_multimap
    #define HashMultiSet        ::__gnu_cxx::hash_multiset
    #define HashSpace           ::__gnu_cxx
#endif

#elif N_COMPILER == N_CPP_GCCE

    #if defined(_LIBCPP_VERSION) || __cplusplus >= 201103L
        #define HashMap         ::std::unordered_map
        #define HashSet         ::std::unordered_set
        #define HashMultiMap    ::std::unordered_multimap
        #define HashMultiSet    ::std::unordered_multiset
        #define HashSpace       ::std
    #else
        #define HashMap         ::std::tr1::unordered_map
        #define HashSet         ::std::tr1::unordered_set
        #define HashMultiMap    ::std::tr1::unordered_multimap
        #define HashMultiSet    ::std::tr1::unordered_multiset
        #define HashSpace       ::std::tr1
    #endif

#else
    
    #if N_COMPILER == N_CPP_MSVC && !defined(_STLP_MSVC)
        #if N_COMPILER_VER >= 1900 // VC++ 10.0
            #define HashMap         ::std::unordered_map
            #define HashSet         ::std::unordered_set
            #define HashMultiMap    ::std::unordered_multimap
            #define HashMultiSet    ::std::unordered_multiset
            #define HashSpace       ::std
        #elif N_COMPILER_VER > 1600
            #define HashMap         ::tr1::hash_map
            #define HashSet         ::tr1::hash_set
            #define HashMultiMap    ::tr1::hash_multimap
            #define HashMultiSet    ::tr1::hash_multiset
            #define HashSpace       ::tr1
        #else
            #define HashMap         ::std::hash_map
            #define HashSet         ::std::hash_set
            #define HashMultiMap    ::std::hash_multimap
            #define HashMultiSet    ::std::hash_multiset
            #define HashSpace       ::std
        #endif
    #else
        #define HashMap             ::std::unordered_map
        #define HashSet             ::std::unordered_set
        #define HashMultiMap        ::std::unordered_multimap
        #define HashMultiSet        ::std::unordered_multiset
        #define HashSpace           ::std
    #endif
#endif

#if N_COMPILER == N_CPP_GNUC && NII_COMP_VER >= 310 && !defined(STLPORT)
    // For gcc 4.3 see http://gcc.gnu.org/gcc-4.3/changes.html
    #if NII_COMP_VER >= 430
        #include <tr1/unordered_map>
    #else
        #include <ext/hash_map>
        namespace __gnu_cxx
        {
            template <> struct hash<void *>
            {
                size_t operator()(void * const & ptr) const
                {
                    return (size_t)ptr;
                }
            };
        }
    #endif
#endif

#ifdef __BORLANDC__
namespace NII
{
    using namespace std;
}
#endif

extern "C"
{
    #include <sys/types.h>
    #include <sys/stat.h>
}

#if N_PLAT == N_PLAT_WIN32
    #undef min
    #undef max
    #if defined(__MINGW32__)
        #include <unistd.h>
    #endif
#elif N_PLAT == N_PLAT_LINUX || N_PLAT == N_PLAT_ANDROID
extern "C"
{
    #include <unistd.h>
    #include <dlfcn.h>
}
#elif N_PLAT == N_PLAT_OSX || N_PLAT == N_PLAT_IOS
extern "C"
{
    #include <unistd.h>
    #include <sys/param.h>
    #include <CoreFoundation/CoreFoundation.h>
}
#endif

#if defined (NII_GCC_VISIBILITY)
    #pragma GCC visibility pop
#endif

#if NII_USE_BOOST
    #include <boost/range.hpp>
#endif

#include "NiiMemConfig.h"

#if N_COMPILER == N_CPP_MSVC
#pragma warning (disable : 4251)
#endif

namespace NII
{
    /** 1.vector(常用)
            (连续的空间存储,可以使用[]操作符)快速的访问随机的元素,能快速的在末尾插入
            元素和末尾取出数据,但是在序列中间岁间的插入,删除元素要慢,而且如果一开始
            分配的空间不够的话,有一个重新分配更大空间,然后拷贝存在性能开销.
        2.deque
            (小片的连续,小片间用链表相连,实际上内部有一个map的指针,因为知道类型,所
            以还是可以使用[],只是速度没有vector快)快速的访问随机的元素,快速的在首
            端和末端插入元素,随机的插入,删除元素要慢,空间的重新分配要比vector快,重
            新分配空间后,原有的元素不需要拷贝.对deque的排序操作,可将deque先复制到
            vector,排序后在复制回deque.
        3.list(常用)
            (每个元素间用链表相连)访问随机元素不如vector快,随机的插入元素比vector
            快,对每个元素分配空间,所以不存在空间不够,重新分配的情况

            技巧: list.end() 和 list.rbegin() 永远都不会改变
        4.set
            内部元素唯一,用(红黑)一棵平衡树结构来存储,因此遍历的时候就排序了,查找也比较
            快的.
        5.map(常用)
            一对一的映射的结合,key不能重复.
        6.stack (可用的容器类型 vector deque list)
            适配器,必须结合其他的容器使用,stl中默认的内部容器是deque.先进后出,只
            有一个出口,不允许遍历.
        7.queue (可用的容器类型 deque list)
            是受限制的deque,内部容器一般使用list较简单.先进先出,不允许遍历.

        下面是选择顺序容器类型的一些准则

        1.  如果需要随机访问一个容器则vector要比list好得多 .

        2.  如果已知要存储元素的个数则vector 又是一个比list好的选择.

        3.  如果需要的不仅只是在容器两端插入和删除元素则list显然要比vector好

        4.  除非需要在容器首部插入和删除元素否则vector要比deque好.

        5.  如果只在容易的首部和尾部插入数据元素,则选择deque.

        6.  如果只需要在读取输入时在容器的中间位置插入元素,然后需要随机访问元素,
            则可考虑输入时将元素读入到一个List容器,接着对此容器重新排写,使其适合
            顺序访问,然后将排序后的list容器复制到一个vector容器中

        选择顺序容器类型的一些准则：

        (1) 随机访问，vector比list好得多;

        (2) 已知道元素的个数，则vector比list强;

        (3) 如果不仅是在容器尾端插入和删除元素，则list比vector强很多;

        (4) 除非需要在容器首部插入和删除元素，则vector比deque好。对于小的数据类型，
            vector的插入大批量数据的性能比list好很多，而大型数据时则相反，解决方
            案是只保留大型数据的指针。Reserve函数容许将容器的容量设置成一个显式指
            定的值，resize函数重新设置容器的长度;每个容器支持一组关系操作符;用户定
            义的类类型必须支持等于操作符，必须支持小于操作符，元素类型必须支持一个
            缺省值(对类类型，指缺省构造函数)
    */
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    // normal
    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
    template <typename T, typename A = StlAlloc<T, N_Alloc> >
    struct deque
    {
#if NII_STLCustomAlloc
        typedef typename std::deque<T, A> type;
#else
        typedef typename std::deque<T> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename T, typename A = StlAlloc<T, N_Alloc> >
    struct vector
    {
#if NII_STLCustomAlloc
        typedef typename std::vector<T, A> type;
#else
        typedef typename std::vector<T> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
        
        template<typename type>
        typename type::iterator remove( type & container, typename type::iterator& iterator )
        {
            const size_t idx = iterator - container.begin();
            *iterator = container.back();
            container.pop_back();

            return container.begin() + idx;
        }
    };

    template <typename T, typename A = StlAlloc<T, N_Alloc> >
    struct list
    {
#if NII_STLCustomAlloc
        typedef typename std::list<T, A> type;
#else
        typedef typename std::list<T> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename T, typename P = std::less<T>, typename A = StlAlloc<T, N_Alloc> >
    struct set
    {
#if NII_STLCustomAlloc
        typedef typename std::set<T, P, A> type;
#else
        typedef typename std::set<T, P> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };
    
    template <typename K, typename H = HashSpace::hash<K>, typename E = std::equal_to<K>,
        typename A = StlAlloc<K, N_Alloc> >
    struct unordered_set
    {
#if NII_STLCustomAlloc
        typedef typename HashSet<K, H, E, A> type;
#else
        typedef typename HashSet<K, H, E> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename K, typename H = HashSpace::hash<K>, typename E = std::equal_to<K>,
              typename A = StlAlloc<K, N_Alloc> >
    struct unordered_multiset
    {
#if NII_STLCustomAlloc
        typedef typename HashMultiSet<K, H, E, A> type;
#else
        typedef typename HashMultiSet<K, H, E> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename T, typename P = std::less<T>, typename A = StlAlloc<T, N_Alloc> >
    struct multiset
    {
#if NII_STLCustomAlloc
        typedef typename std::multiset<T, P, A> type;
#else
        typedef typename std::multiset<T, P> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename K, typename V, typename P = std::less<K>, typename A = StlAlloc<std::pair<const K, V>, N_Alloc> >
    struct map
    {
#if NII_STLCustomAlloc
        typedef typename std::map<K, V, P, A> type;
#else
        typedef typename std::map<K, V, P> type;
#endif
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename K, typename V, typename P = std::less<K>, typename A = StlAlloc<std::pair<const K, V>, N_Alloc> >
    struct multimap
    {
#if NII_STLCustomAlloc
        typedef typename std::multimap<K, V, P, A> type;
#else
        typedef typename std::multimap<K, V, P> type;
#endif
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };
    
    template <typename K, typename V, typename H = HashSpace::hash<K>,
              typename E = std::equal_to<K>,
              typename A = StlAlloc<std::pair<const K, V>, N_Alloc> >
    struct unordered_map
    {
#if NII_STLCustomAlloc
        typedef typename HashMap<K, V, H, E, A> type;
#else
        typedef typename HashMap<K, V, H, E> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename K, typename V, typename H = HashSpace::hash<K>,
              typename E = std::equal_to<K>,
              typename A = StlAlloc<std::pair<const K, V>, N_Alloc> >
    struct unordered_multimap
    {
#if NII_STLCustomAlloc
        typedef typename HashMultiMap<K, V, H, E, A> type;
#else
        typedef typename HashMultiMap<K, V, H, E> type;
#endif
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    // simd
    //-------------------------------------------------------------------------------
    //-------------------------------------------------------------------------------
    template <typename T, typename A = StlAlloc<T, N_AlignAlloc(16)> >
    struct Sdeque
    {
        typedef typename std::deque<T, A> type;
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename T, typename A = StlAlloc<T, N_AlignAlloc(16)> >
    struct Svector
    {
        typedef typename std::vector<T, A> type;
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename T, typename A = StlAlloc<T, N_AlignAlloc(16)> >
    struct Slist
    {
        typedef typename std::list<T, A> type;
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename T, typename P = std::less<T>, typename A = StlAlloc<T, N_AlignAlloc(16)> >
    struct Sset
    {
        typedef typename std::set<T, P, A> type;
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename T, typename P = std::less<T>, typename A = StlAlloc<T, N_AlignAlloc(16)> >
    struct Smultiset
    {
        typedef typename std::multiset<T, P, A> type;
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };
    
    template <typename K, typename H = HashSpace::hash<K>, typename E = std::equal_to<K>,
              typename A = StlAlloc<T, N_AlignAlloc(16)> >
    struct Sunordered_set
    {
        typedef typename HashSet<K, H, E, A> type;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename K, typename H = HashSpace::hash<K>, typename E = std::equal_to<K>,
              typename A = StlAlloc<T, N_AlignAlloc(16)> >
    struct Sunordered_multiset
    {
        typedef typename HashMultiSet<K, H, E, A> type;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename K, typename V, typename P = std::less<K>, typename A = StlAlloc<std::pair<const K, V>, N_AlignAlloc(16)> >
    struct Smap
    {
        typedef typename std::map<K, V, P, A> type;
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename K, typename V, typename P = std::less<K>, typename A = StlAlloc<std::pair<const K, V>, N_AlignAlloc(16)> >
    struct Smultimap
    {
        typedef typename std::multimap<K, V, P, A> type;
        typedef typename type::value_type value;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };
    
    template <typename K, typename V, typename H = HashSpace::hash<K>,
              typename E = std::equal_to<K>,
              typename A = StlAlloc<std::pair<const K, V>, N_AlignAlloc(16)> >
    struct Sunordered_map
    {
        typedef typename HashMap<K, V, H, E, A> type;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    template <typename K, typename V, typename H = HashSpace::hash<K>,
              typename E = std::equal_to<K>,
              typename A = StlAlloc<std::pair<const K, V>, N_AlignAlloc(16)> >
    struct Sunordered_multimap
    {
        typedef typename HashMultiMap<K, V, H, E, A> type;
        typedef typename type::iterator iterator;
        typedef typename type::const_iterator const_iterator;
    };

    // normal
    #define Ndeque(value, name)         typedef deque<value>::type name
    #define Nvector(value, name)        typedef vector<value>::type name
    #define Nlist(value, name)          typedef list<value>::type name
    #define Nset(value, name)           typedef set<value>::type name
    #define Nmap(key, value, name)      typedef map<key, value>::type name
    #define Nmultimap(key, value, name) typedef multimap<key, value>::type name

    #define NSdeque(value, name)        typedef Sdeque<value>::type name
    #define NSvector(value, name)       typedef Svector<value>::type name
    #define NSlist(value, name)         typedef Slist<value>::type name
    #define NSset(value, name)          typedef Sset<value>::type name
    #define NSmap(key, value, name)     typedef Smap<key, value>::type name
    #define NSmultimap(key, value, name)typedef Smultimap<key, value>::type name

    #define Npair(f, s)                 std::make_pair(f, s)
    // simd
    #define NdequeType(value)           typedef deque<value>::type
    #define NvectorType(value)          typedef vector<value>::type
    #define NlistType(value)            typedef list<value>::type
    #define NsetType(value)             typedef set<value>::type
    #define NmapType(key,value)         typedef map<key, value>::type
    #define NmultimapType(key, value)   typedef multimap<key, value>::type

    #define NSdequeType(value)          typedef Sdeque<value>::type
    #define NSvectorType(value)         typedef Svector<value>::type
    #define NSlistType(value)           typedef Slist<value>::type
    #define NSsetType(value)            typedef Sset<value>::type
    #define NSmapType(key,value)        typedef Smap<key, value>::type
    #define NSmulTypeimapT(key, value)  typedef Smultimap<key, value>::type

    template <typename type, typename itype, typename vtype> class Iterator
    {
    public:
        typedef vtype ValueType;
        typedef vtype * PointerType;
        typedef itype iterator;
        typedef itype const_iterator;

        Iterator(itype begin, itype end) :
            mBegin(begin),
            mCurrent(begin),
            mEnd(end) {}

        inline bool isNextValid() const { return mCurrent != mEnd; }

        inline void next()              { ++mCurrent; }

        inline const itype & begin()    { return mBegin; }

        inline itype & current()        { return mCurrent; }

        inline const itype & end()      { return mEnd; }
    private:
        Iterator();
    protected:
        itype mBegin;
        itype mCurrent;
        itype mEnd;
    };

    /** 数组容器
    @version NIIEngine 3.0.0
    */
    template <typename type, typename itype>
    class VectorContainer : public Iterator<type, itype, typename type::value_type>
    {
    public:
        typedef typename Iterator<type, itype, typename type::value_type>::ValueType ValueType;
        typedef typename Iterator<type, itype, typename type::value_type>::PointerType PointerType;

        VectorContainer(itype begin, itype end) :
            Iterator<type, itype, typename type::value_type>(begin, end) {}

        inline ValueType getCurrent() const         { return *(this->mCurrent); }

        inline PointerType getCurrentPtr()  const   { return &(*(this->mCurrent)); }

        inline ValueType getAndNext()               { return *(this->mCurrent++); }
    };

    /** 数组迭代
    @param[in] type 容器
    */
    template <typename type>class VectorIterator : public VectorContainer<type, typename type::iterator>
    {
    public:
        VectorIterator(typename type::iterator begin, typename type::iterator end) :
            VectorContainer<type, typename type::iterator>(begin, end) {}

        explicit VectorIterator(type & c) :
            VectorContainer<type, typename type::iterator>(c.begin(), c.end()) {}
    };

    /** 静态数组迭代
    @version NIIEngine 3.0.0
    */
    template <typename type> class ConstVectorIterator : public VectorContainer<type, typename type::const_iterator>
    {
    public:
        ConstVectorIterator(typename type::const_iterator begin, typename type::const_iterator end) :
            VectorContainer<type, typename type::const_iterator>(begin, end) {}

        explicit ConstVectorIterator(const type & c) :
            VectorContainer<type, typename type::const_iterator>(c.begin(), c.end()) {}
    };

    /** 映射容器
    @version NIIEngine 3.0.0
    */
    template <typename type, typename itype>
    class MapContainer : public Iterator<type, itype, typename type::mapped_type>
    {
    public:
        typedef typename Iterator<type, itype, typename type::mapped_type>::ValueType ValueType;
        typedef typename Iterator<type, itype, typename type::mapped_type>::PointerType PointerType;

        typedef typename type::value_type PairType;
        typedef typename type::key_type KeyType;

        MapContainer(itype begin, itype end) :
            Iterator<type, itype, typename type::mapped_type>(begin, end) {}

        inline KeyType getCurrentKey() const            { return this->mCurrent->first; }

        inline ValueType getCurrentValue() const        { return this->mCurrent->second; }

        inline const PointerType getCurrentValuePtr()const{ return &(this->mCurrent->second); }

        inline ValueType getAndNext()                   { return ((this->mCurrent++)->second); }
    };

    /** 映射迭代
    @version NIIEngine 3.0.0
    */
    template <typename type>
    class MapIterator : public MapContainer<type, typename type::iterator>
    {
    public:
        MapIterator(typename type::iterator begin, typename type::iterator end) :
            MapContainer<type, typename type::iterator>(begin, end) {}

        explicit MapIterator(type & c) :
            MapContainer<type, typename type::iterator>(c.begin(), c.end()) {}
    };

    /** 静态映射迭代
    @version NIIEngine 3.0.0
    */
    template <typename type> class ConstMapIterator : public MapContainer<type, typename type::const_iterator>
    {
    public:
        ConstMapIterator(typename type::const_iterator begin, typename type::const_iterator end) :
            MapContainer<type, typename type::const_iterator>(begin, end) {}

        explicit ConstMapIterator(const type & c) :
            MapContainer<type, typename type::const_iterator>(c.begin(), c.end()) {}
    };

    /** 范围迭代
    @version NIIEngine 3.0.0
    */
    template <typename T> class iterator_range
    {
#if !NII_USE_BOOST
    public:
        typedef T iterator;
        typedef T const_iterator;
        typedef iterator_range<T> type;
#else
    public: 
        typedef boost::iterator_range<T> type;
#endif
        iterator_range(T b, T e) : mBegin(b), mEnd(e) {}

        inline bool equal(const iterator_range & other) const       { return mBegin == other.mBegin && mEnd == other.mEnd; }

        inline bool operator== (const iterator_range & rhs) const   { return equal(rhs); }

        inline bool operator!= (const iterator_range & rhs) const   { return !operator==(rhs); }

        inline T begin() const      { return mBegin; }

        inline T end() const        { return mEnd; }

        inline bool empty() const   { return mBegin = mEnd; }
    private:
        T mBegin, mEnd;
    };

    /** 数组范围
    @version NIIEngine 3.0.0
    */
    template<typename T> struct VectorRange : public iterator_range<typename T::iterator>::type
    {
        VectorRange(T & c) : iterator_range<typename T::iterator>::type(c.begin(), c.end()) {}

        VectorRange(typename T::iterator begin, typename T::iterator end) :
            iterator_range<typename T::iterator>::type(begin, end) {}

        inline bool operator== (const VectorRange & o) const{ return equal(o);}

        inline bool operator!= (const VectorRange & o) const{ return !equal(o);}

        inline operator VectorIterator<T>() { return VectorIterator<T>(this->begin(), this->end()); }

        inline operator ConstVectorIterator<T>() { return ConstVectorIterator<T>(this->begin(), this->end()); }
    };

    /** 静态数组范围
    @version NIIEngine 3.0.0
    */
    template<typename T> struct ConstVectorRange : public iterator_range<typename T::const_iterator>::type
    {
        ConstVectorRange(const T & c) :
            iterator_range<typename T::const_iterator>::type(c.begin(), c.end()) {}

        ConstVectorRange(typename T::iterator begin, typename T::iterator end) :
            iterator_range<typename T::const_iterator>::type(begin, end) {}

        ConstVectorRange(typename T::const_iterator begin, typename T::const_iterator end) :
            iterator_range<typename T::const_iterator>::type(begin, end) {}

        ConstVectorRange(const VectorRange<T> & o) :
            iterator_range<typename T::const_iterator>::type(o.begin(), o.end()) {}

        inline bool operator== (const ConstVectorRange & o) const { return equal(o); }

        inline bool operator!= (const ConstVectorRange & o) const { return !equal(o); }

        inline operator ConstVectorIterator<T>() { return  ConstVectorIterator<T>(this->begin(), this->end()); }
    };

    /** 映射容器
    @version NIIEngine 3.0.0
    */
    template<typename T> struct MapRange : public iterator_range<typename T::iterator>::type
    {
        MapRange(T & c) : iterator_range<typename T::iterator>::type(c.begin(), c.end()) {}
        MapRange(typename T::iterator begin, typename T::iterator end) :
            iterator_range<typename T::iterator>::type(begin, end) {}

        inline bool operator== (const MapRange & o) const { return equal(o); }
        inline bool operator!= (const MapRange & o) const { return !equal(o); }

        inline operator MapIterator<T>() { return MapIterator<T>(this->begin(), this->end()); }

        inline operator ConstMapIterator<T>() { return ConstMapIterator<T>(this->begin(), this->end()); }
    };

    /** 静态映射容器
    @version NIIEngine 3.0.0
    */
    template<typename T> struct ConstMapRange : public iterator_range<typename T::const_iterator>::type
    {
        ConstMapRange(const T & c) :
            iterator_range<typename T::const_iterator>::type(c.begin(), c.end()) {}

        ConstMapRange(typename T::iterator b, typename T::iterator e) :
            iterator_range<typename T::const_iterator>::type(b, e) {}

        ConstMapRange(typename T::const_iterator b, typename T::const_iterator e) :
            iterator_range<typename T::const_iterator>::type(b, e) {}

        ConstMapRange(const MapRange<T> & o) :
            iterator_range<typename T::const_iterator>::type(o.begin(), o.end()) {}

        inline bool operator== (const ConstMapRange & o) const { return equal(o); }

        inline bool operator!= (const ConstMapRange & o) const { return !equal(o); }

        inline operator ConstMapIterator<T>() { return  ConstMapIterator<T>(this->begin(), this->end()); }
    };

    /** 控制范围类
    @version NIIEngine 3.0.0
    */
    template <typename T> static T Clamp(T val, T min, T max)
    {
        return std::max(std::min(val, max), min);
    }
    
    /// 简版std::vector
    template <typename T> class vectorL
    {
    public:
        typedef T value_type;
        typedef T * iterator;
        typedef const T * const_iterator;
    public:
        vectorL() : mData(0), mSize(0), mCapacity(0) {}

        vectorL(const vectorL<T> & o) :
            mSize(o.mSize),
            mCapacity(o.mSize)
        {
            mData = N_alloc_t(T, mSize);
            for(size_t i = 0; i < mSize; ++i)
            {
                new (&mData[i]) T(o.mData[i]);
            }
        }

        vectorL(size_t count) :
            mSize(0),
            mCapacity(count)
        {
            mData = N_alloc_t(T, count);
        }

        vectorL(size_t count, const T & value) :
            mSize(count),
            mCapacity(count)
        {
            mData = N_alloc_t(T, count);
            for(size_t i = 0; i < count; ++i)
            {
                new (&mData[i]) T(value);
            }
        }

        ~vectorL()
        {
            destroy();
        }
        
        void operator = (const vectorL<T> & o)
        {
            if(&o != this)
            {
                DeleteArray(mData, mSize);
                N_free(mData);

                mSize = o.mSize;
                mCapacity = o.mSize;

                mData = N_alloc_t(T, mSize);
                for(size_t i = 0; i < mSize; ++i)
                {
                    new (&mData[i]) T(o.mData[i]);
                }
            }
        }

        inline size_t size() const              { return mSize; }
        inline size_t capacity() const          { return mCapacity; }

        void push_back(const T & val)
        {
            expand(1);
            new (&mData[mSize]) T(val);
            ++mSize;
        }

        void pop_back()
        {
            N_assert1(mSize > 0);
            --mSize;
            mData[mSize].~T();
        }

        iterator insert(iterator where, const T & val)
        {
            const size_t idx = (where - mData);

            expand(1);

            memmove(mData + idx + 1, mData + idx, (mSize - idx) *  sizeof(T));
            new (&mData[idx]) T(val);
            ++mSize;

            return mData + idx;
        }

        iterator insert(iterator pos, const_iterator o_begin, const_iterator o_end)
        {
            size_t idx = (pos - mData);

            const size_t otherSize = o_end - o_begin;

            expand(otherSize);

            memmove(mData + idx + otherSize, mData + idx, (mSize - idx) *  sizeof(T));

            while(o_begin != o_end)
                *pos++ = *o_begin++;
            mSize += otherSize;

            return mData + idx;
        }

        void append(const_iterator o_begin, const_iterator o_end)
        {
            expand(o_end - o_begin);

            memcpy(mData + mSize, o_begin, (o_end - o_begin) *  sizeof(T));
            mSize += o_end - o_begin;
        }

        iterator erase(iterator pos, bool destory = true)
        {
            size_t idx = (pos - mData);
            if(destory)
            {
                pos->~T();
            }
            memmove(mData + idx, mData + idx + 1, (mSize - idx - 1) * sizeof(T));
            --mSize;

            return mData + idx;
        }

        iterator erase(iterator first, iterator last, bool destory = true)
        {
            N_assert1(first <= last && last <= end());

            const size_t idx      = (first - mData);
            const size_t idxNext  = (last - mData);
            if(destory)
            {
                while(first != last)
                {
                    first->~T();
                    ++first;
                }
            }
            memmove(mData + idx, mData + idxNext, (mSize - idxNext) * sizeof(T));
            mSize -= idxNext - idx;

            return mData + idx;
        }

        void clear(bool destory = true)
        {
            if(destory)
            {
                DeleteArray(mData, mSize);
            }
            mSize = 0;
        }

        inline bool empty() const               { return mSize == 0; }

        void reserve(size_t cnt)
        {
            if(cnt > mCapacity)
            {
                mCapacity = cnt;
                T * data = N_alloc_t(T, mCapacity);
                memcpy(data, mData, mSize * sizeof(T));
                N_free(mData);
                mData = data;
            }
        }

        void resize(size_t newSize, const T & value = T(), bool destory = true)
        {
            if(newSize > mSize)
            {
                expand(newSize - mSize);
                for(size_t i = mSize; i < newSize; ++i)
                {
                    new (&mData[i]) T(value);
                }
            }
            else if(destory)
            {
                for(size_t i = newSize; i < mSize; ++i)
                    mData[i].~T();
            }

            mSize = newSize;
        }

        inline T & operator [] (size_t idx)
        {
            N_assert1(idx < mSize);
            return mData[idx];
        }

        inline const T & operator [] (size_t idx) const
        {
            N_assert1(idx < mSize);
            return mData[idx];
        }

        inline T & back()
        {
            N_assert1(mSize > 0);
            return mData[mSize - 1];
        }

        inline const T & back() const
        {
            N_assert1(mSize > 0);
            return mData[mSize - 1];
        }

        inline T & front()
        {
            N_assert1(mSize > 0);
            return mData[0];
        }

        inline const T & front() const
        {
            N_assert1(mSize > 0);
            return mData[0];
        }

        inline iterator begin()                 { return mData; }
        inline const_iterator begin() const     { return mData; }
        inline iterator end()                   { return mData + mSize; }
        inline const_iterator end() const       { return mData + mSize; }

        void swap(vectorL<T> & o)
        {
            std::swap(mData, o.mData);
            std::swap(mSize, o.mSize);
            std::swap(mCapacity, o.mCapacity);
        }
        
        void destroy()
        {
            DeleteArray(mData, mSize);
            N_free(mData);
            mSize = 0;
            mCapacity = 0;
            mData = 0;
        }
    protected:
        void expand(size_t cnt)
        {
            if(mSize + cnt > mCapacity)
            {
                mCapacity = std::max<size_t>(mSize + cnt, mCapacity + (mCapacity >> 1u) + 1u);
                T * data = N_alloc_t(T, mCapacity);
                if(mData)
                {
                    memcpy(data, mData, mSize * sizeof(T));
                    N_free(mData);
                }
                mData = data;
            }
        }

        T * mData;
        size_t mSize;
        size_t mCapacity;
    };
    
    /// 简版std::array
    template <typename T, size_t Size> class arrayL
    {
    public:
        typedef T value_type;
        typedef T * iterator;
        typedef const T * const_iterator;
    public:
        arrayL() : mSize(0) {}

        template <size_t C> arrayL(const arrayL<T, C> & copy) :
            mSize(copy.mSize)
        {
            N_assert1(copy.mSize <= Size);
            for(size_t i = 0; i < mSize; ++i)
            {
                new (&mData[i]) T(copy.mData[i]);
            }
        }

        template <size_t C> void operator = (const arrayL<T, C> & copy)
        {
            if(copy.data() != this->data())
            {
                for(size_t i = 0; i < mSize; ++i)
                    mData[i] = T();

                N_assert1(copy.size() <= Size);
                mSize = copy.size();

                const T * srcData = copy.data();

                for(size_t i = 0; i < mSize; ++i)
                {
                    new (&mData[i]) T(srcData[i]);
                }
            }
        }

        arrayL(size_t count, const T & value) :
            mSize(count)
        {
            for(size_t i = 0; i < count; ++i)
                new (&mData[i]) T( value );
        }

        ~arrayL()
        {
            destroy();
        }

        inline size_t size() const                      { return mSize; }
        inline size_t capacity() const                  { return Size; }
        inline T * data()                               { return mData; }
        inline const T * data() const                   { return mData; }

        inline void push_back( const T& val )
        {
            N_assert1(mSize < Size);
            new (&mData[mSize]) T( val );
            ++mSize;
        }

        inline void pop_back()
        {
            N_assert1(mSize > 0);
            --mSize;
            mData[mSize] = T();
        }

        iterator insert( iterator pos, const T & val )
        {
            size_t idx = (pos - mData);

            N_assert1(mSize < Size);

            memmove( mData + idx + 1, mData + idx, (mSize - idx) *  sizeof(T) );
            new (&mData[idx]) T( val );
            ++mSize;

            return mData + idx;
        }

        iterator insert(iterator pos, const_iterator o_begin, const_iterator o_end)
        {
            size_t idx = (pos - mData);

            const size_t otherSize = o_end - o_begin;

            N_assert1(mSize + otherSize <= Size);

            memmove(mData + idx + otherSize, mData + idx, (mSize - idx) *  sizeof(T));

            while(o_begin != o_end )
                *pos++ = *o_begin++;
            mSize += otherSize;

            return mData + idx;
        }

        void append(const_iterator o_begin, const_iterator o_end)
        {
            N_assert1(mSize + (o_end - o_begin) <= Size);

            memcpy(mData + mSize, o_begin, (o_end - o_begin) *  sizeof(T));
            mSize += o_end - o_begin;
        }

        iterator erase(iterator pos)
        {
            size_t idx = (pos - mData);
            pos = T();
            memmove( mData + idx, mData + idx + 1, (mSize - idx - 1) * sizeof(T) );
            --mSize;

            return mData + idx;
        }

        iterator erase(iterator first, iterator last, bool destory = true)
        {
            N_assert1(first <= last && last <= end());

            size_t idx      = (first - mData);
            size_t idxNext  = (last - mData);
            if(destory)
            {
                while(first != last)
                {
                    first = T();
                    ++first;
                }
            }
            memmove(mData + idx, mData + idxNext, (mSize - idxNext) * sizeof(T));
            mSize -= idxNext - idx;

            return mData + idx;
        }

        void clear(bool destory = true)
        {
            if(destory)
            {
                for(size_t i = 0; i < mSize; ++i)
                    mData[i] = T();
            }
            mSize = 0;
        }

        inline bool empty() const                      { return mSize == 0; }

        void resize(size_t newSize, const T & value = T(), bool destory = true)
        {
            if(newSize > mSize)
            {
                N_assert1(newSize <= Size);
                for(size_t i = mSize; i < newSize; ++i)
                {
                    new (&mData[i]) T(value);
                }
            }
            else if(destory)
            {
                for(size_t i = newSize; i < mSize; ++i)
                {
                    mData[i] = T();
                }
            }

            mSize = newSize;
        }

        inline T & operator [] (size_t idx)
        {
            N_assert1(idx < mSize);
            return mData[idx];
        }

        inline const T & operator [] (size_t idx) const
        {
            N_assert1(idx < mSize);
            return mData[idx];
        }

        inline T & back()
        {
            N_assert1(mSize > 0);
            return mData[mSize-1];
        }

        inline const T & back() const
        {
            N_assert1(mSize > 0);
            return mData[mSize-1];
        }

        inline T & front()
        {
            N_assert1(mSize > 0);
            return mData[0];
        }

        inline const T & front() const
        {
            N_assert1(mSize > 0);
            return mData[0];
        }

        inline iterator begin()                     { return mData; }
        inline const_iterator begin() const         { return mData; }
        inline iterator end()                       { return mData + mSize; }
        inline const_iterator end() const           { return mData + mSize; }
        
        inline void destroy()                       { mSize = 0; }
    protected:
        T mData[Size];
        size_t mSize;
    };
    
    /** 双重映射vector
    @remark 用于优化映射,减少数据移动
    @version NIIEngine 6.0.0
    */
    template <typename T> class DMappingVector
    {
    public:
        typedef T value_type;
        typedef T * iterator;
        typedef const T * const_iterator;
        typedef vectorL<Nidx> Mapping;
    public:
        DMappingVector() : mData(0), mSize(0), mCapacity(0) {}

        DMappingVector(const DMappingVector<T> & o) :
            mSize(o.mSize),
            mCapacity(o.mSize)
        {
            mData = N_alloc_t(T, mSize);
            for(size_t i = 0; i < mSize; ++i)
            {
                new (&mData[i]) T(o.mData[i]);
            }
        }

        DMappingVector(size_t count) :
            mSize(0),
            mCapacity(count)
        {
            mData = N_alloc_t(T, count);
        }

        DMappingVector(size_t count, const T & value) :
            mSize(count),
            mCapacity(count)
        {
            mData = N_alloc_t(T, count);
            for(size_t i = 0; i < count; ++i)
            {
                new (&mData[i]) T(value);
            }
        }

        ~DMappingVector()
        {
            destroy();
        }
        
        void operator = (const DMappingVector<T> & o)
        {
            if(&o != this)
            {
                for(size_t i = 0; i < mSize; ++i)
                    mData[i].~T();
                N_free(mData);

                mSize = o.mSize;
                mCapacity = o.mSize;

                mData = N_alloc_t(T, mSize);
                for(size_t i = 0; i < mSize; ++i)
                {
                    new (&mData[i]) T(o.mData[i]);
                }
            }
        }

        inline size_t size() const              { return mSize; }
        inline size_t capacity() const          { return mCapacity; }

        void push_back(const T & val)
        {
            expand(1);
            new (&mData[mSize]) T(val);
            ++mSize;
        }

        void pop_back()
        {
            N_assert1(mSize > 0);
            --mSize;
            mData[mSize].~T();
        }

        iterator insert(iterator where, const T & val)
        {
            const size_t idx = (where - mData);

            expand(1);

            memmove(mData + idx + 1, mData + idx, (mSize - idx) *  sizeof(T));
            new (&mData[idx]) T(val);
            ++mSize;

            return mData + idx;
        }

        iterator insert(iterator pos, const_iterator o_begin, const_iterator o_end)
        {
            size_t idx = (pos - mData);

            const size_t otherSize = o_end - o_begin;

            expand(otherSize);

            memmove(mData + idx + otherSize, mData + idx, (mSize - idx) *  sizeof(T));

            while(o_begin != o_end)
                *pos++ = *o_begin++;
            mSize += otherSize;

            return mData + idx;
        }

        void append(const_iterator o_begin, const_iterator o_end)
        {
            expand(o_end - o_begin);

            memcpy(mData + mSize, o_begin, (o_end - o_begin) *  sizeof(T));
            mSize += o_end - o_begin;
        }

        iterator erase(iterator pos, bool destory = true)
        {
            size_t idx = (pos - mData);
            if(destory)
            {
                pos->~T();
            }
            memmove(mData + idx, mData + idx + 1, (mSize - idx - 1) * sizeof(T));
            --mSize;

            return mData + idx;
        }

        iterator erase(iterator first, iterator last, bool destory = true)
        {
            N_assert1(first <= last && last <= end());

            const size_t idx      = (first - mData);
            const size_t idxNext  = (last - mData);
            if(destory)
            {
                while(first != last)
                {
                    first->~T();
                    ++first;
                }
            }
            memmove(mData + idx, mData + idxNext, (mSize - idxNext) * sizeof(T));
            mSize -= idxNext - idx;

            return mData + idx;
        }

        void clear(bool destory = true)
        {
            if(destory)
            {
                for(size_t i = 0; i < mSize; ++i)
                    mData[i].~T();
            }
            mSize = 0;
        }

        inline bool empty() const               { return mSize == 0; }

        void reserve(size_t cnt)
        {
            if(cnt > mCapacity)
            {
                mCapacity = cnt;
                T * data = N_alloc_t(T, mCapacity);
                memcpy(data, mData, mSize * sizeof(T));
                N_free(mData);
                mData = data;
            }
        }

        void resize(size_t newSize, const T & value = T(), bool destory = true)
        {
            if(newSize > mSize)
            {
                expand(newSize - mSize);
                for(size_t i = mSize; i < newSize; ++i)
                {
                    new (&mData[i]) T(value);
                }
            }
            else if(destory)
            {
                for(size_t i = newSize; i < mSize; ++i)
                    mData[i].~T();
            }

            mSize = newSize;
        }

        inline T & operator [] (size_t idx)
        {
            N_assert1(idx < mSize);
            return mData[idx];
        }

        inline const T & operator [] (size_t idx) const
        {
            N_assert1(idx < mSize);
            return mData[idx];
        }

        inline T & back()
        {
            N_assert1(mSize > 0);
            return mData[mSize - 1];
        }

        inline const T & back() const
        {
            N_assert1(mSize > 0);
            return mData[mSize - 1];
        }

        inline T & front()
        {
            N_assert1(mSize > 0);
            return mData[0];
        }

        inline const T & front() const
        {
            N_assert1(mSize > 0);
            return mData[0];
        }

        inline iterator begin()                 { return mData; }
        inline const_iterator begin() const     { return mData; }
        inline iterator end()                   { return mData + mSize; }
        inline const_iterator end() const       { return mData + mSize; }

        void swap(DMappingVector<T> & o)
        {
            std::swap(mData, o.mData);
            std::swap(mSize, o.mSize);
            std::swap(mCapacity, o.mCapacity);
        }
        
        void destroy()
        {
            for(size_t i = 0; i < mSize; ++i)
                mData[i].~T();
            N_free(mData);
            mSize = 0;
            mCapacity = 0;
            mData = 0;
        }
    protected:
        void expand(size_t cnt)
        {
            if(mSize + cnt > mCapacity)
            {
                mCapacity = std::max<size_t>(mSize + cnt, mCapacity + (mCapacity >> 1u) + 1u);
                T * data = N_alloc_t(T, mCapacity);
                if(mData)
                {
                    memcpy(data, mData, mSize * sizeof(T));
                    N_free(mData);
                }
                mData = data;
            }
        }

        T * mData;
        Mapping mMapping;
        size_t mSize;
        size_t mCapacity;
    };
}
#endif
