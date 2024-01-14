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

#ifndef __HASH_H
#define __HASH_H

#include "NiiNetPreInclude.h"
#include <string.h> // memmove
#include "RakString.h"

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
    struct HashIndex
    {
        Nui32 primaryIndex;
        Nui32 secondaryIndex;
        bool IsInvalid(void) const {return primaryIndex==(Nui32) -1;}
        void SetInvalid(void) {primaryIndex=(Nui32) -1; secondaryIndex=(Nui32) -1;}
    };

    /// \brief Using a string as a identifier for a node, store an allocated pointer to that node
    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    class _EngineAPI Hash
    {
    public:
        /// Default constructor
        Hash();

        // Destructor
        ~Hash();

        void Push(key_type key, const data_type & input);
        data_type* Peek(key_type key );
        bool Pop(data_type & out, key_type key);
        bool RemoveAtIndex(HashIndex index);
        bool Remove(key_type key);
        HashIndex GetIndexOf(key_type key);
        bool HasData(key_type key);
        data_type& ItemAtIndex(const HashIndex &index);
        key_type  KeyAtIndex(const HashIndex &index);
        void GetAsList(DataStructures::List<data_type> &itemList,DataStructures::List<key_type > &keyList) const;
        Nui32 Size(void) const;

        /// \brief Clear the list
        void Clear();

        struct Node
        {
            Node(key_type strIn, const data_type &_data) {string=strIn; mData=_data;}
            key_type  string;
            data_type mData;
            // Next in the list for this key
            Node *next;
        };

    protected:
        void ClearIndex(Nui32 index);
        Node **nodeList;
        Nui32 size;
    };

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    Hash<key_type, data_type, HASH_SIZE, hashFunction>::Hash()
    {
        nodeList=0;
        size=0;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    Hash<key_type, data_type, HASH_SIZE, hashFunction>::~Hash()
    {
        Clear();
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    void Hash<key_type, data_type, HASH_SIZE, hashFunction>::Push(key_type key, const data_type &input)
    {
        Nul hashIndex = (*hashFunction)(key) % HASH_SIZE;
        if (nodeList==0)
        {
            nodeList= N_alloc_t(Node *, HASH_SIZE);
            memset(nodeList,0,sizeof(Node *)*HASH_SIZE);
        }

        Node * newNode = N_new Node(key, input);
        newNode->next = nodeList[hashIndex];
        nodeList[hashIndex] = newNode;

        size++;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    data_type* Hash<key_type, data_type, HASH_SIZE, hashFunction>::Peek(key_type key )
    {
        if (nodeList==0)
            return 0;

        Nul hashIndex = (*hashFunction)(key) % HASH_SIZE;
        Node *node = nodeList[hashIndex];
        while (node!=0)
        {
            if (node->string==key)
                return &node->mData;
            node=node->next;
        }
        return 0;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    bool Hash<key_type, data_type, HASH_SIZE, hashFunction>::Pop(data_type& out, key_type key)
    {
        if (nodeList==0)
            return false;

        Nul hashIndex = (*hashFunction)(key) % HASH_SIZE;
        Node *node = nodeList[hashIndex];
        if (node==0)
            return false;
        if (node->next==0)
        {
            // Only one item.
            if (node->string==key)
            {
                // Delete last item
                out=node->mData;
                ClearIndex(hashIndex);
                return true;
            }
            else
            {
                // Single item doesn't match
                return false;
            }
        }
        else if (node->string==key)
        {
            // First item does match, but more than one item
            out=node->mData;
            nodeList[hashIndex]=node->next;
            N_delete node;
            size--;
            return true;
        }

        Node *last=node;
        node=node->next;

        while (node!=0)
        {
            // First item does not match, but subsequent item might
            if (node->string==key)
            {
                out=node->mData;
                // Skip over subsequent item
                last->next=node->next;
                // Delete existing item
                N_delete node;
                size--;
                return true;
            }
            last=node;
            node=node->next;
        }
        return false;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    bool Hash<key_type, data_type, HASH_SIZE, hashFunction>::RemoveAtIndex(HashIndex index)
    {
        if (index.IsInvalid())
            return false;

        Node *node = nodeList[index.primaryIndex];
        if (node==0)
            return false;
        if (node->next==0)
        {
            // Delete last item
            ClearIndex(index.primaryIndex,file,line);
            return true;
        }
        else if (index.secondaryIndex==0)
        {
            // First item does match, but more than one item
            nodeList[index.primaryIndex]=node->next;
            N_delete node;
            size--;
            return true;
        }

        Node *last=node;
        node=node->next;
        --index.secondaryIndex;

        while (index.secondaryIndex!=0)
        {
            last=node;
            node=node->next;
            --index.secondaryIndex;
        }

        // Skip over subsequent item
        last->next=node->next;
        // Delete existing item
        N_delete node;
        size--;
        return true;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    bool Hash<key_type, data_type, HASH_SIZE, hashFunction>::Remove(key_type key)
    {
        return RemoveAtIndex(GetIndexOf(key),file,line);
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    HashIndex Hash<key_type, data_type, HASH_SIZE, hashFunction>::GetIndexOf(key_type key)
    {
        if (nodeList==0)
        {
            HashIndex temp;
            temp.SetInvalid();
            return temp;
        }
        HashIndex idx;
        idx.primaryIndex=(*hashFunction)(key) % HASH_SIZE;
        Node *node = nodeList[idx.primaryIndex];
        if (node==0)
        {
            idx.SetInvalid();
            return idx;
        }
        idx.secondaryIndex=0;
        while (node!=0)
        {
            if (node->string==key)
            {
                return idx;
            }
            node=node->next;
            idx.secondaryIndex++;
        }

        idx.SetInvalid();
        return idx;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    bool Hash<key_type, data_type, HASH_SIZE, hashFunction>::HasData(key_type key)
    {
        return GetIndexOf(key).IsInvalid()==false;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    data_type& Hash<key_type, data_type, HASH_SIZE, hashFunction>::ItemAtIndex(const HashIndex &index)
    {
        Node *node = nodeList[index.primaryIndex];
        N_assert(node);
        Nui32 i;
        for (i=0; i < index.secondaryIndex; i++)
        {
            node=node->next;
            N_assert(node);
        }
        return node->mData;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    key_type  Hash<key_type, data_type, HASH_SIZE, hashFunction>::KeyAtIndex(const HashIndex &index)
    {
        Node *node = nodeList[index.primaryIndex];
        N_assert(node);
        Nui32 i;
        for (i=0; i < index.secondaryIndex; i++)
        {
            node=node->next;
            N_assert(node);
        }
        return node->string;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    void Hash<key_type, data_type, HASH_SIZE, hashFunction>::Clear()
    {
        if (nodeList)
        {
            Nui32 i;
            for (i=0; i < HASH_SIZE; i++)
                ClearIndex(i,file,line);
            N_free(nodeList);
            nodeList=0;
            size=0;
        }
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    void Hash<key_type, data_type, HASH_SIZE, hashFunction>::ClearIndex(Nui32 index)
    {
        Node *node = nodeList[index];
        Node *next;
        while (node)
        {
            next=node->next;
            N_delete node;
            node=next;
            size--;
        }
        nodeList[index]=0;
    }

    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    void Hash<key_type, data_type, HASH_SIZE, hashFunction>::GetAsList(DataStructures::List<data_type> &itemList,DataStructures::List<key_type > &keyList) const
    {
        if (nodeList==0)
            return;
        itemList.Clear(false);
        keyList.Clear(false);

        Node *node;
        Nui32 i;
        for (i=0; i < HASH_SIZE; i++)
        {
            if (nodeList[i])
            {
                node=nodeList[i];
                while (node)
                {
                    itemList.Push(node->mData,file,line);
                    keyList.Push(node->string,file,line);
                    node=node->next;
                }
            }
        }
    }
    template <class key_type, class data_type, Nui32 HASH_SIZE, Nul (*hashFunction)(const key_type &) >
    Nui32 Hash<key_type, data_type, HASH_SIZE, hashFunction>::Size(void) const
    {
        return size;
    }
}
#endif
