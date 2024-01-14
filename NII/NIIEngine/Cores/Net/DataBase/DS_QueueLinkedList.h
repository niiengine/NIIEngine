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

#ifndef __QUEUE_LINKED_LIST_H
#define __QUEUE_LINKED_LIST_H

#include "NiiNetPreInclude.h"
#include "DS_LinkedList.h" 

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
    /// \brief A queue implemented using a linked list.  Rarely used.
    template <class QueueType>
    class _EngineAPI QueueLinkedList
    {
    
    public:
        QueueLinkedList();
        QueueLinkedList( const QueueLinkedList& original_copy );
        bool operator= ( const QueueLinkedList& original_copy );
        QueueType Pop( void );
        QueueType& Peek( void );
        QueueType& EndPeek( void );
        void Push( const QueueType& input );
        Nui32 Size( void );
        void Clear( void );
        void Compress( void );
        
    private:
        LinkedList<QueueType> mData;
    };
    
    template <class QueueType>
    QueueLinkedList<QueueType>::QueueLinkedList()
    {
    }
    
    template <class QueueType>
    inline Nui32 QueueLinkedList<QueueType>::Size()
    {
        return mData.Size();
    }
    
    template <class QueueType>
    inline QueueType QueueLinkedList<QueueType>::Pop( void )
    {
        mData.Beginning();
        return ( QueueType ) mData.Pop();
    }
    
    template <class QueueType>
    inline QueueType& QueueLinkedList<QueueType>::Peek( void )
    {
        mData.Beginning();
        return ( QueueType ) mData.Peek();
    }
    
    template <class QueueType>
    inline QueueType& QueueLinkedList<QueueType>::EndPeek( void )
    {
        mData.End();
        return ( QueueType ) mData.Peek();
    }
    
    template <class QueueType>
    void QueueLinkedList<QueueType>::Push( const QueueType& input )
    {
        mData.End();
        mData.Add( input );
    }
    
    template <class QueueType>
    QueueLinkedList<QueueType>::QueueLinkedList( const QueueLinkedList& original_copy )
    {
        mData = original_copy.mData;
    }
    
    template <class QueueType>
    bool QueueLinkedList<QueueType>::operator= ( const QueueLinkedList& original_copy )
    {
        if ( ( &original_copy ) == this )
            return false;
            
        mData = original_copy.mData;
    }
    
    template <class QueueType>
    void QueueLinkedList<QueueType>::Clear ( void )
    {
        mData.Clear();
    }
} // End namespace

#endif
