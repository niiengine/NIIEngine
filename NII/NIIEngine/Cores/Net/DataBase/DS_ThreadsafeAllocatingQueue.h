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

#ifndef __THREADSAFE_ALLOCATING_QUEUE
#define __THREADSAFE_ALLOCATING_QUEUE

#include "DS_Queue.h"
#include "NiiThreadManager.h"

namespace DataStructures
{

template <class structureType>
class _EngineAPI ThreadsafeAllocatingQueue
{
public:
    // Queue operations
    void Push(structureType * s);
    structureType * PopInaccurate(void);
    structureType * Pop(void);
    void SetPageSize(int size);
    bool IsEmpty(void);
    structureType * operator[] (Nui32 position);
    void RemoveAtIndex(Nui32 position);
    Nui32 Size(void);

    // Memory pool operations
    structureType * Allocate();
    void Deallocate(structureType * s);
    void Clear();
protected:
    ThreadMutex memoryPoolMutex;
    Queue<structureType *> queue;
    ThreadMutex queueMutex;
};

template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::Push(structureType * s)
{
    queueMutex.Lock();
    queue.Push(s);
    queueMutex.Unlock();
}

template <class structureType>
structureType * ThreadsafeAllocatingQueue<structureType>::PopInaccurate(void)
{
    structureType * s;
    if(queue.IsEmpty())
        return 0;
    queueMutex.Lock();
    if(queue.IsEmpty() == false)
        s = queue.Pop();
    else
        s = 0;
    queueMutex.Unlock();
    return s;
}

template <class structureType>
structureType *ThreadsafeAllocatingQueue<structureType>::Pop(void)
{
    structureType *s;
    queueMutex.Lock();
    if (queue.IsEmpty())
    {
        queueMutex.Unlock();
        return 0;
    }
    s=queue.Pop();
    queueMutex.Unlock();
    return s;
}

template <class structureType>
structureType * ThreadsafeAllocatingQueue<structureType>::Allocate()
{
    structureType * s;
    memoryPoolMutex.Lock();
    s = N_new structureType;
    memoryPoolMutex.Unlock();
    return s;
}
template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::Deallocate(structureType * s)
{
    // Call delete operator, memory pool doesn't do this
    s->~structureType();
    memoryPoolMutex.Lock();
    N_delete s;
    memoryPoolMutex.Unlock();
}

template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::Clear()
{
    memoryPoolMutex.Lock();
    for (Nui32 i=0; i < queue.Size(); i++)
    {
        queue[i]->~structureType();
        N_delete queue[i];
    }
    queue.Clear(file, line);
    memoryPoolMutex.Unlock();
}

template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::SetPageSize(int size)
{
}

template <class structureType>
bool ThreadsafeAllocatingQueue<structureType>::IsEmpty(void)
{
    bool isEmpty;
    queueMutex.Lock();
    isEmpty=queue.IsEmpty();
    queueMutex.Unlock();
    return isEmpty;
}

template <class structureType>
structureType * ThreadsafeAllocatingQueue<structureType>::operator[] ( Nui32 position )
{
    structureType *s;
    queueMutex.Lock();
    s=queue[position];
    queueMutex.Unlock();
    return s;
}

template <class structureType>
void ThreadsafeAllocatingQueue<structureType>::RemoveAtIndex( Nui32 position )
{
    queueMutex.Lock();
    queue.RemoveAtIndex(position);
    queueMutex.Unlock();
}

template <class structureType>
Nui32 ThreadsafeAllocatingQueue<structureType>::Size( void )
{
    Nui32 s;
    queueMutex.Lock();
    s=queue.Size();
    queueMutex.Unlock();
    return s;
}

}

#endif