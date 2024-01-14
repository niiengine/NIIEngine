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

#ifndef __RAKNET_HEAP_H
#define __RAKNET_HEAP_H

#include "NiiNetPreInclude.h"
#include "DS_List.h"

#ifdef _MSC_VER
#pragma warning( push )
#endif

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
    template <class weight_type, class data_type, bool isMaxHeap>
    class _EngineAPI Heap
    {
    public:
        struct HeapNode
        {
            HeapNode() {}
            HeapNode(const weight_type &w, const data_type &d) : weight(w), mData(d) {}
            weight_type weight; // I'm assuming key is a native numerical type - NIIf or int
            data_type mData;
        };

        Heap();
        ~Heap();
        void Push(const weight_type &weight, const data_type &data);
        /// Call before calling PushSeries, for a new series of items
        void StartSeries(void) {optimizeNextSeriesPush=false;}
        /// If you are going to push a list of items, where the weights of the items on the list are in order and follow the heap order, PushSeries is faster than Push()
        void PushSeries(const weight_type &weight, const data_type &data);
        data_type Pop(const Nui32 startingIndex);
        data_type Peek(const Nui32 startingIndex=0) const;
        weight_type PeekWeight(const Nui32 startingIndex=0) const;
        void Clear(bool doNotDeallocateSmallBlocks);
        data_type& operator[] ( const Nui32 position ) const;
        Nui32 Size(void) const;

    protected:
        Nui32 LeftChild(const Nui32 i) const;
        Nui32 RightChild(const Nui32 i) const;
        Nui32 Parent(const Nui32 i) const;
        void Swap(const Nui32 i, const Nui32 j);
        DataStructures::List<HeapNode> heap;
        bool optimizeNextSeriesPush;
    };

    template  <class weight_type, class data_type, bool isMaxHeap>
        Heap<weight_type, data_type, isMaxHeap>::Heap()
    {
        optimizeNextSeriesPush=false;
    }

    template  <class weight_type, class data_type, bool isMaxHeap>
        Heap<weight_type, data_type, isMaxHeap>::~Heap()
    {
    }

    template  <class weight_type, class data_type, bool isMaxHeap>
    void Heap<weight_type, data_type, isMaxHeap>::PushSeries(const weight_type &weight, const data_type &data)
    {
        if (optimizeNextSeriesPush==false)
        {
            // If the weight of what we are inserting is greater than / less than in order of the heap of every sibling and sibling of parent, then can optimize next push
            Nui32 currentIndex = heap.Size();
            Nui32 parentIndex;
            if (currentIndex>0)
            {
                for (parentIndex = Parent(currentIndex); parentIndex < currentIndex; parentIndex++)
                {
#ifdef _MSC_VER
#pragma warning(disable:4127)   // conditional expression is constant
#endif
                    if (isMaxHeap)
                    {
                        // Every child is less than its parent
                        if (weight>heap[parentIndex].weight)
                        {
                            // Can't optimize
                            Push(weight,data,file,line);
                            return;
                        }
                    }
                    else
                    {
                        // Every child is greater than than its parent
                        if (weight<heap[parentIndex].weight)
                        {
                            // Can't optimize
                            Push(weight,data,file,line);
                            return;
                        }
                    }
                }
            }

            // Parent's subsequent siblings and this row's siblings all are less than / greater than inserted element. Can insert all further elements straight to the end
            heap.Insert(HeapNode(weight, data), file, line);
            optimizeNextSeriesPush=true;
        }
        else
        {
            heap.Insert(HeapNode(weight, data), file, line);
        }
    }

    template  <class weight_type, class data_type, bool isMaxHeap>
    void Heap<weight_type, data_type, isMaxHeap>::Push(const weight_type &weight, const data_type &data)
    {
        Nui32 currentIndex = heap.Size();
        Nui32 parentIndex;
        heap.Insert(HeapNode(weight, data), file, line);
        while (currentIndex!=0)
        {
            parentIndex = Parent(currentIndex);
#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
            if (isMaxHeap)
            {
                if (heap[parentIndex].weight < weight)
                {
                    Swap(currentIndex, parentIndex);
                    currentIndex=parentIndex;
                }
                else
                    break;
            }
            else
            {
                if (heap[parentIndex].weight > weight)
                {
                    Swap(currentIndex, parentIndex);
                    currentIndex=parentIndex;
                }
                else
                    break;
            }
        }
    }

    template  <class weight_type, class data_type, bool isMaxHeap>
    data_type Heap<weight_type, data_type, isMaxHeap>::Pop(const Nui32 startingIndex)
    {
        // While we have children, swap out with the larger of the two children.

        // This line will N_assert on an empty heap
        data_type returnValue=heap[startingIndex].mData;

        // Move the last element to the head, and re-heapify
        heap[startingIndex]=heap[heap.Size()-1];

        Nui32 currentIndex,leftChild,rightChild;
        weight_type currentWeight;
        currentIndex=startingIndex;
        currentWeight=heap[startingIndex].weight;
        heap.RemoveFromEnd();

#ifdef _MSC_VER
#pragma warning( disable : 4127 ) // warning C4127: conditional expression is constant
#endif
        while (1)
        {
            leftChild=LeftChild(currentIndex);
            rightChild=RightChild(currentIndex);
            if (leftChild >= heap.Size())
            {
                // Done
                return returnValue;
            }
            if (rightChild >= heap.Size())
            {
                // Only left node.
                if ((isMaxHeap==true && currentWeight < heap[leftChild].weight) ||
                    (isMaxHeap==false && currentWeight > heap[leftChild].weight))
                        Swap(leftChild, currentIndex);

                return returnValue;
            }
            else
            {
                // Swap with the bigger/smaller of the two children and continue
                if (isMaxHeap)
                {
                    if (heap[leftChild].weight <= currentWeight && heap[rightChild].weight <= currentWeight)
                        return returnValue;

                    if (heap[leftChild].weight > heap[rightChild].weight)
                    {
                        Swap(leftChild, currentIndex);
                        currentIndex=leftChild;
                    }
                    else
                    {
                        Swap(rightChild, currentIndex);
                        currentIndex=rightChild;
                    }
                }
                else
                {
                    if (heap[leftChild].weight >= currentWeight && heap[rightChild].weight >= currentWeight)
                        return returnValue;

                    if (heap[leftChild].weight < heap[rightChild].weight)
                    {
                        Swap(leftChild, currentIndex);
                        currentIndex=leftChild;
                    }
                    else
                    {
                        Swap(rightChild, currentIndex);
                        currentIndex=rightChild;
                    }
                }
            }
        }
    }

    template  <class weight_type, class data_type, bool isMaxHeap>
    inline data_type Heap<weight_type, data_type, isMaxHeap>::Peek(const Nui32 startingIndex) const
    {
        return heap[startingIndex].mData;
    }

    template  <class weight_type, class data_type, bool isMaxHeap>
    inline weight_type Heap<weight_type, data_type, isMaxHeap>::PeekWeight(const Nui32 startingIndex) const
    {
        return heap[startingIndex].weight;
    }

    template  <class weight_type, class data_type, bool isMaxHeap>
        void Heap<weight_type, data_type, isMaxHeap>::Clear(bool doNotDeallocateSmallBlocks)
    {
        heap.Clear(doNotDeallocateSmallBlocks, file, line);
    }

    template <class weight_type, class data_type, bool isMaxHeap>
    inline data_type& Heap<weight_type, data_type, isMaxHeap>::operator[] (const Nui32 position) const
    {
        return heap[position].mData;
    }
    template <class weight_type, class data_type, bool isMaxHeap>
        Nui32 Heap<weight_type, data_type, isMaxHeap>::Size(void) const
    {
        return heap.Size();
    }

    template <class weight_type, class data_type, bool isMaxHeap>
    inline Nui32 Heap<weight_type, data_type, isMaxHeap>::LeftChild(const Nui32 i) const
    {
        return i*2+1;
    }

    template <class weight_type, class data_type, bool isMaxHeap>
    inline Nui32 Heap<weight_type, data_type, isMaxHeap>::RightChild(const Nui32 i) const
    {
        return i*2+2;
    }

    template <class weight_type, class data_type, bool isMaxHeap>
    inline Nui32 Heap<weight_type, data_type, isMaxHeap>::Parent(const Nui32 i) const
    {
#ifdef _DEBUG
        N_assert(i!=0);
#endif
        return (i-1)/2;
    }

    template <class weight_type, class data_type, bool isMaxHeap>
    void Heap<weight_type, data_type, isMaxHeap>::Swap(const Nui32 i, const Nui32 j)
    {
        HeapNode temp;
        temp=heap[i];
        heap[i]=heap[j];
        heap[j]=temp;
    }
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

#endif
