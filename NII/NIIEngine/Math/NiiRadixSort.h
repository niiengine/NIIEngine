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

#ifndef _NII_RadixSort_H_
#define _NII_RadixSort_H_

#include "NiiPreInclude.h"

namespace NII
{
    /** 执行一个基数排序(基于字节值快速比较无排序)的各种类型的标准STL容器的类.
    @remark 
        基数排序是一个非常快的排序算法.它不使用比较,因而能够打破理论最小为O(N * logN)
        的复杂性.基数排序的复杂度为O(k*N),其中k是一个常量.注意基数排序不是就地方式,它
        需要额外的存储空间,因此它是交换内存获得速度.该复制开销意味着,只有相当大的数据
        集才会更快,所以建议你只使用了至少几百个项的集合
    @par
        这是一个模板类,允许它处理各种容器,以及多种类型的值排序.在构造器中除了提供容器
        和值类型,你也许需要提供一个函子对象(检索值比较在列表的每个项),例如,如果你有一
        个Bibble类对象实例的值的std::vector,你想要Bibble::getDoobrie()排序,你必须先
        创建一个函子,像这样:
    @code
        struct BibbleSortFunctor
        {
            NIIf operator()(const Bibble& val) const
            {
                return val.getDoobrie();
            }
        }
    @cond 
        然后,你需要用容器类型名字,容器类型中的值类型和你要排序的值类型声明一个RadixSort
        类,之后可以调用的排序函子,列如
    @code
        RadixSort<BibbleList, Bibble, NIIf> radixSorter;
        BibbleSortFunctor functor;

        radixSorter.sort(myBibbleList, functor);
    @cond
        你因该尝试重新使用RadixSort实例,因此重复分配内部存储,避免这个问题
    @note
        基数排序往往仅关联无符号整数的值.我们的实现可以处理无符号和有符号的整形,以
        及浮点(其他的基数排序一般不支持),双精度不支持,如果你希望使用这个排序常规,
        你要去实现转换为NIIf函子对象.
    */
    template <class TContainer, class TContainerValueType, typename TCompValueType>
    class RadixSort
    {
    public:
        typedef typename TContainer::iterator ContainerIter;
    public:
        RadixSort() {}
        ~RadixSort() {}

        /** 主要的排序功能
        @param[in] container 一个类型容器,在声明时声明
        @param[in] func 一个仿函数,当给定一个容器值时返回值的比较
        */
        template <class TFunction> void sort(TContainer & container, TFunction func)
        {
            if (container.empty())
                return;

            // Set up the sort areas
            mSortSize = static_cast<NIIi>(container.size());
            mSortArea1.resize(container.size());
            mSortArea2.resize(container.size());

            // Copy data now (we need constant iterators for sorting)
            mTmpContainer = container;

            mNumPasses = sizeof(TCompValueType);

            // Counter pass
            // Initialise the counts
            NIIi p;
            for (p = 0; p < mNumPasses; ++p)
                memset(mCounters[p], 0, sizeof(NIIi) * 256);

            // Perform alpha pass to count
            ContainerIter i = mTmpContainer.begin();
            TCompValueType prevValue = func.operator()(*i);
            bool needsSorting = false;
            for (NIIi u = 0; i != mTmpContainer.end(); ++i, ++u)
            {
                // get sort value
                TCompValueType val = func.operator()(*i);
                // cheap check to see if needs sorting (temporal coherence)
                if (!needsSorting && val < prevValue)
                    needsSorting = true;

                // Create a sort entry
                mSortArea1[u].key = val;
                mSortArea1[u].iter = i;

                // increase counters
                for (p = 0; p < mNumPasses; ++p)
                {
                    Nui8 byteVal = getByte(p, val);
                    mCounters[p][byteVal]++;
                }
                prevValue = val;
            }

            // early exit if already sorted
            if (!needsSorting)
                return;

            // Sort passes
            mSrc = &mSortArea1;
            mDest = &mSortArea2;

            for (p = 0; p < mNumPasses - 1; ++p)
            {
                sortPass(p);
                // flip src/dst
                SortVector * tmp = mSrc;
                mSrc = mDest;
                mDest = tmp;
            }
            // Final pass may differ, make polymorphic
            finalPass(p, prevValue);

            // Copy everything back
            NIIi c = 0;
            for (i = container.begin();
                i != container.end(); ++i, ++c)
            {
                *i = *((*mDest)[c].iter);
            }
        }
    protected:
        void sortPass(NIIi byteIndex)
        {
            // Calculate offsets
            // Basically this just leaves gaps for duplicate entries to fill
            mOffsets[0] = 0;
            for (NIIi i = 1; i < 256; ++i)
            {
                mOffsets[i] = mOffsets[i-1] + mCounters[byteIndex][i-1];
            }

            // Sort pass
            for (NIIi i = 0; i < mSortSize; ++i)
            {
                Nui8 byteVal = getByte(byteIndex, (*mSrc)[i].key);
                (*mDest)[mOffsets[byteVal]++] = (*mSrc)[i];
            }

        }

        template <typename T> void finalPass(NIIi byteIndex, T val)
        {
            // default is to do normal pass
            sortPass(byteIndex);
        }

        // 特殊情况下的NIIi签名
        void finalPass(NIIi byteIndex, NIIi val)
        {
            NIIi numNeg = 0;
            // all negative values are in entries 128+ in most significant byte
            for (NIIi i = 128; i < 256; ++i)
            {
                numNeg += mCounters[byteIndex][i];
            }
            // Calculate offsets - positive ones start at the number of negatives
            // do positive numbers
            mOffsets[0] = numNeg;
            for (NIIi i = 1; i < 128; ++i)
            {
                mOffsets[i] = mOffsets[i-1] + mCounters[byteIndex][i-1];
            }
            // Do negative numbers (must start at zero)
            // No need to invert ordering, already correct (-1 is highest number)
            mOffsets[128] = 0;
            for (NIIi i = 129; i < 256; ++i)
            {
                mOffsets[i] = mOffsets[i-1] + mCounters[byteIndex][i-1];
            }

            // Sort pass
            for (NIIi i = 0; i < mSortSize; ++i)
            {
                Nui8 byteVal = getByte(byteIndex, (*mSrc)[i].key);
                (*mDest)[mOffsets[byteVal]++] = (*mSrc)[i];
            }
        }

        // 特殊情况下的NIIf签名
        void finalPass(NIIi byteIndex, NIIf val)
        {
            // floats need to be special cased since negative numbers will come
            // after positives (high bit = sign) and will be in reverse order
            // (no ones-complement of the +ve value)
            NIIi numNeg = 0;
            // all negative values are in entries 128+ in most significant byte
            for (NIIi i = 128; i < 256; ++i)
            {
                numNeg += mCounters[byteIndex][i];
            }
            // Calculate offsets - positive ones start at the number of negatives
            // do positive numbers normally
            mOffsets[0] = numNeg;
            for (NIIi i = 1; i < 128; ++i)
            {
                mOffsets[i] = mOffsets[i-1] + mCounters[byteIndex][i-1];
            }
            // Do negative numbers (must start at zero)
            // Also need to invert ordering
            // In order to preserve the stability of the sort (essential since
            // we rely on previous bytes already being sorted) we have to count
            // backwards in our offsets from
            mOffsets[255] = mCounters[byteIndex][255];
            for (NIIi i = 254; i > 127; --i)
            {
                mOffsets[i] = mOffsets[i+1] + mCounters[byteIndex][i];
            }

            // Sort pass
            for (NIIi i = 0; i < mSortSize; ++i)
            {
                Nui8 byteVal = getByte(byteIndex, (*mSrc)[i].key);
                if (byteVal > 127)
                {
                    // -ve; pre-decrement since offsets set to count
                    (*mDest)[--mOffsets[byteVal]] = (*mSrc)[i];
                }
                else
                {
                    // +ve
                    (*mDest)[mOffsets[byteVal]++] = (*mSrc)[i];
                }
            }
        }

        inline Nui8 getByte(NIIi byteIndex, TCompValueType val)
        {
#if N_ENDIAN == N_ENDIAN_L
            return ((Nui8*)(&val))[byteIndex];
#else
            return ((Nui8*)(&val))[mNumPasses - byteIndex - 1];
#endif
        }
    protected:
        /// 值的透明通道记数(直方图) 4个值的,所以我们最大可以实现32位值的基数排序
        NIIi mCounters[4][256];
        /// Beta-pass 便移量
        NIIi mOffsets[256];
        NIIi mSortSize;
        NIIi mNumPasses;

        struct SortEntry
        {
            TCompValueType key;
            ContainerIter iter;
            SortEntry() {}
            SortEntry(TCompValueType k, ContainerIter it) : key(k), iter(it) {}
        };
        /// 临时排序存储
        typedef std::vector<SortEntry, StlAlloc<SortEntry, N_Alloc> > SortVector;
        SortVector mSortArea1;
        SortVector mSortArea2;
        SortVector * mSrc;
        SortVector * mDest;
        TContainer mTmpContainer; // initial copy
    };
}
#endif