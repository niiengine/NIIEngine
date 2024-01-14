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

#ifndef __RAKNET_ORDERED_CHANNEL_HEAP_H_
#define __RAKNET_ORDERED_CHANNEL_HEAP_H_

#include "DS_Heap.h"
#include "DS_Map.h"
#include "DS_Queue.h"
#include "Rand.h"

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)=defaultMapKeyComparison<channel_key_type> >
    class _EngineAPI OrderedChannelHeap
    {
    public:
        static void IMPLEMENT_DEFAULT_COMPARISON(void) {DataStructures::defaultMapKeyComparison<channel_key_type>(channel_key_type(),channel_key_type());}

        OrderedChannelHeap();
        ~OrderedChannelHeap();
        void Push(const channel_key_type &channelID, const heap_data_type &data);
        void PushAtHead(const Nui32 index, const channel_key_type &channelID, const heap_data_type &data);
        heap_data_type Pop(const Nui32 startingIndex=0);
        heap_data_type Peek(const Nui32 startingIndex) const;
        void AddChannel(const channel_key_type &channelID, const NIId weight);
        void RemoveChannel(channel_key_type channelID);
        void Clear(void);
        heap_data_type& operator[] ( const Nui32 position ) const;
        Nui32 ChannelSize(const channel_key_type &channelID);
        Nui32 Size(void) const;

        struct QueueAndWeight
        {
            DataStructures::Queue<NIId> randResultQueue;
            NIId weight;
            bool signalDeletion;
        };

        struct HeapChannelAndData
        {
            HeapChannelAndData() {}
            HeapChannelAndData(const channel_key_type &_channel, const heap_data_type &_data) : mData(_data), channel(_channel) {}
            heap_data_type mData;
            channel_key_type channel;
        };

    protected:
        DataStructures::Map<channel_key_type, QueueAndWeight*, channel_key_comparison_func> map;
        DataStructures::Heap<NIId, HeapChannelAndData, true> heap;
        void GreatestRandResult(void);
    };

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
        OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::OrderedChannelHeap()
    {
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
        OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::~OrderedChannelHeap()
    {
        Clear();
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
    void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Push(const channel_key_type &channelID, const heap_data_type &data)
    {
        PushAtHead(MAX_UNSIGNED_LONG, channelID, data);
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
    void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::GreatestRandResult(void)
    {
        NIId greatest;
        Nui32 i;
        greatest=0.0;
        for (i=0; i < map.Size(); i++)
        {
            if (map[i]->randResultQueue.Size() && map[i]->randResultQueue[0]>greatest)
                greatest=map[i]->randResultQueue[0];
        }
        return greatest;
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
    void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::PushAtHead(const Nui32 index, const channel_key_type &channelID, const heap_data_type &data)
    {
        // If an N_assert hits here then this is an unknown channel.  Call AddChannel first.
        QueueAndWeight *queueAndWeight=map.Get(channelID);
        NIId maxRange, minRange, rnd;
        if (queueAndWeight->randResultQueue.Size()==0)
        {
            // Set maxRange to the greatest random number waiting to be returned, rather than 1.0 necessarily
            // This is so weights are scaled similarly among channels.  For example, if the head weight for a used channel was .25
            // and then we added another channel, the new channel would need to choose between .25 and 0
            // If we chose between 1.0 and 0, it would be 1/.25 (4x) more likely to be at the head of the heap than it should be
            maxRange=GreatestRandResult();
            if (maxRange==0.0)
                maxRange=1.0;
            minRange=0.0;
        }
        else if (index >= queueAndWeight->randResultQueue.Size())
        {
            maxRange=queueAndWeight->randResultQueue[queueAndWeight->randResultQueue.Size()-1]*.99999999;
            minRange=0.0;
        }
        else
        {
            if (index==0)
            {
                maxRange=GreatestRandResult();
                if (maxRange==queueAndWeight->randResultQueue[0])
                    maxRange=1.0;
            }
            else if (index >= queueAndWeight->randResultQueue.Size())
                maxRange=queueAndWeight->randResultQueue[queueAndWeight->randResultQueue.Size()-1]*.99999999;
            else
                maxRange=queueAndWeight->randResultQueue[index-1]*.99999999;

            minRange=maxRange=queueAndWeight->randResultQueue[index]*1.00000001;
        }
        
#ifdef _DEBUG
        N_assert(maxRange!=0.0);
#endif
        rnd=frandomMT() * (maxRange - minRange);
        if (rnd==0.0)
            rnd=maxRange/2.0;

        if (index >= queueAndWeight->randResultQueue.Size())
            queueAndWeight->randResultQueue.Push(rnd);
        else
            queueAndWeight->randResultQueue.PushAtHead(rnd, index);

        heap.Push(rnd*queueAndWeight->weight, HeapChannelAndData(channelID, data));
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
    heap_data_type OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Pop(const Nui32 startingIndex)
    {
        N_assert(startingIndex < heap.Size());

        QueueAndWeight *queueAndWeight=map.Get(heap[startingIndex].channel);
        if (startingIndex!=0)
        {
            // Ugly - have to count in the heap how many nodes have the same channel, so we know where to delete from in the queue
            Nui32 indiceCount=0;
            Nui32 i;
            for (i=0; i < startingIndex; i++)
                if (channel_key_comparison_func(heap[i].channel,heap[startingIndex].channel)==0)
                    indiceCount++;
            queueAndWeight->randResultQueue.RemoveAtIndex(indiceCount);
        }
        else
        {
            // TODO - ordered channel heap uses progressively lower values as items are inserted.  But this won't give relative ordering among channels.  I have to renormalize after every pop.
            queueAndWeight->randResultQueue.Pop();
        }

        // Try to remove the channel after every pop, because doing so is not valid while there are elements in the list.
        if (queueAndWeight->signalDeletion)
            RemoveChannel(heap[startingIndex].channel);

        return heap.Pop(startingIndex).mData;
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
    heap_data_type OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Peek(const Nui32 startingIndex) const
    {
        HeapChannelAndData heapChannelAndData = heap.Peek(startingIndex);
        return heapChannelAndData.mData;
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
    void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::AddChannel(const channel_key_type &channelID, NIId weight)
    {
        QueueAndWeight * qaw = N_new QueueAndWeight();
        qaw->weight=weight;
        qaw->signalDeletion=false;
        map.SetNew(channelID, qaw);
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
        void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::RemoveChannel(channel_key_type channelID)
    {
        if (map.Has(channelID))
        {
            Nui32 i;
            i=map.GetIndexAtKey(channelID);
            if (map[i]->randResultQueue.Size()==0)
            {
                N_delete map[i];
                map.RemoveAtIndex(i);
            }
            else
            {
                // Signal this channel for deletion later, because the heap has nodes with this channel right now
                map[i]->signalDeletion=true;
            }
        }
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
        Nui32 OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Size(void) const
    {
        return heap.Size();
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
        heap_data_type& OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::operator[]( const Nui32 position ) const
    {
        return heap[position].mData;
    }


    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
        Nui32 OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::ChannelSize(const channel_key_type &channelID)
    {
        QueueAndWeight *queueAndWeight=map.Get(channelID);
        return queueAndWeight->randResultQueue.Size();
    }

    template <class channel_key_type, class heap_data_type, int (*channel_key_comparison_func)(const channel_key_type&, const channel_key_type&)>
        void OrderedChannelHeap<channel_key_type, heap_data_type, channel_key_comparison_func>::Clear(void)
    {
        Nui32 i;
        for (i=0; i < map.Size(); i++)
            N_delete map[i];
        map.Clear();
        heap.Clear();
    }
}

#endif
