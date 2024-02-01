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

#ifndef __LIST_H_
#define __LIST_H_

#include "NiiNetPreInclude.h"
#include <string.h> // memmove

/// Maximum Nui32
static const Nui32 MAX_UNSIGNED_LONG = 4294967295U;

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
    /// \brief Array based implementation of a list.
    /// \note ONLY USE THIS FOR SHALLOW COPIES.  I don't bother with operator= to improve performance.
    template <class list_type>
    class _EngineAPI List
    {
    public:
        List();
        ~List();

        /// \brief Copy constructor.
        /// \param[in]  original_copy The list to duplicate
        List(const List & original_copy);

        /// \brief Assign one list to another.
        List & operator= (const List & original_copy);

        /// \brief Access an element by its index in the array.
        /// \param[in]  position The index into the array.
        /// \return The element at position \a position.
        list_type & operator[] ( const Nui32 position ) const;

        /// \brief Access an element by its index in the array.
        /// \param[in]  position The index into the array.
        /// \return The element at position \a position.
        list_type & Get ( const Nui32 position ) const;

        /// \brief Push an element at the end of the stack.
        /// \param[in] input The new element.
        void Push(const list_type & input);

        /// \brief Pop an element from the end of the stack.
        /// \pre Size()>0
        /// \return The element at the end.
        list_type & Pop(void);

        /// \brief Insert an element at position \a position in the list.
        /// \param[in] input The new element.
        /// \param[in] position The position of the new element.
        void Insert(const list_type & input, const Nui32 position);

        /// \brief Insert at the end of the list.
        /// \param[in] input The new element.
        void Insert(const list_type & input);

        /// \brief Replace the value at \a position by \a input.
        /// \details If the size of the list is less than @em position, it increase the capacity of
        /// the list and fill slot with @em filler.
        /// \param[in] input The element to replace at position @em position.
        /// \param[in] filler The element use to fill new allocated capacity.
        /// \param[in] position The position of input in the list.
        void Replace(const list_type & input, const list_type filler, const Nui32 position);

        /// \brief Replace the last element of the list by \a input.
        /// \param[in] input The element used to replace the last element.
        void Replace(const list_type & input);

        /// \brief Delete the element at position \a position.
        /// \param[in] position The index of the element to delete
        void RemoveAtIndex(const Nui32 position);

        /// \brief Delete the element at position \a position.
        /// \note - swaps middle with end of list, only use if list order does not matter
        /// \param[in] position The index of the element to delete
        void RemoveAtIndexFast(const Nui32 position);

        /// \brief Delete the element at the end of the list.
        void RemoveFromEnd(const Nui32 num = 1);

        /// \brief Returns the index of the specified item or MAX_UNSIGNED_LONG if not found.
        /// \param[in] input The element to check for
        /// \return The index or position of @em input in the list.
        /// \retval MAX_UNSIGNED_LONG The object is not in the list
        /// \retval [Integer] The index of the element in the list
        Nui32 GetIndexOf(const list_type & input) const;

        /// \return The number of elements in the list
        Nui32 Size(void) const;

        /// \brief Clear the list
        void Clear(bool doNotDeallocateSmallBlocks);

        /// \brief Preallocate the list, so it needs fewer reallocations at runtime.
        void Preallocate(Nui32 countNeeded);

        /// \brief Frees overallocated members, to use the minimum memory necessary.
        /// \attention
        /// This is a slow operation
        void Compress();
    private:
        /// An array of user values
        list_type* listArray;

        /// Number of elements in the list
        Nui32 list_size;

        /// Size of \a array
        Nui32 allocation_size;
    };
    template <class list_type>
        List<list_type>::List()
    {
        allocation_size = 0;
        listArray = 0;
        list_size = 0;
    }

    template <class list_type>
        List<list_type>::~List()
    {
        if (allocation_size>0)
            N_free(listArray);
    }


    template <class list_type>
        List<list_type>::List(const List & original_copy)
    {
        // Allocate memory for copy
        if (original_copy.list_size == 0)
        {
            list_size = 0;
            allocation_size = 0;
        }
        else
        {
            listArray = N_alloc_t(list_type, original_copy.list_size);

            for ( Nui32 counter = 0; counter < original_copy.list_size; ++counter)
                listArray[counter] = original_copy.listArray[counter];

            // Don't call constructors, assignment operators, etc.
            //memcpy(listArray, original_copy.listArray, original_copy.list_size*sizeof(list_type));

            list_size = allocation_size = original_copy.list_size;
        }
    }

    template <class list_type>
        List<list_type> & List<list_type>::operator= (const List & original_copy)
    {
        if ((&original_copy) != this)
        {
            Clear(false);

            // Allocate memory for copy
            if (original_copy.list_size == 0)
            {
                list_size = 0;
                allocation_size = 0;
            }

            else
            {
                listArray = N_alloc_t(list_type, original_copy.list_size);

                for ( Nui32 counter = 0; counter < original_copy.list_size; ++counter )
                    listArray[ counter ] = original_copy.listArray[ counter ];
                // Don't call constructors, assignment operators, etc.
                //memcpy(listArray, original_copy.listArray, original_copy.list_size*sizeof(list_type));

                list_size = allocation_size = original_copy.list_size;
            }
        }

        return *this;
    }


        template <class list_type>
            inline list_type& List<list_type>::operator[] ( const Nui32 position ) const
        {
        #ifdef _DEBUG
            if (position>=list_size)
            {
                N_assert ( position < list_size );
            }
        #endif
            return listArray[ position ];
        }

        // Just here for debugging
        template <class list_type>
        inline list_type& List<list_type>::Get ( const Nui32 position ) const
        {
            return listArray[ position ];
        }

        template <class list_type>
        void List<list_type>::Push(const list_type &input)
        {
            Insert(input, file, line);
        }

        template <class list_type>
        inline list_type& List<list_type>::Pop(void)
        {
#ifdef _DEBUG
            N_assert(list_size>0);
#endif
            --list_size;
            return listArray[list_size];
        }

    template <class list_type>
    void List<list_type>::Insert(const list_type & input, const Nui32 position)
    {
#ifdef _DEBUG
        if (position>list_size)
        {
            N_assert( position <= list_size );
        }
#endif

        // Reallocate list if necessary
        if ( list_size == allocation_size )
        {
            // allocate twice the currently allocated memory
            list_type * new_array;

            if ( allocation_size == 0 )
                allocation_size = 16;
            else
                allocation_size *= 2;

            new_array = N_alloc_t(list_type, allocation_size);

            // copy old array over
            for ( Nui32 counter = 0; counter < list_size; ++counter )
                new_array[ counter ] = listArray[ counter ];

            // Don't call constructors, assignment operators, etc.
            //memcpy(new_array, listArray, list_size*sizeof(list_type));

            // set old array to point to the newly allocated and twice as large array
            N_free(listArray);

            listArray = new_array;
        }

        // Move the elements in the list to make room
        for ( Nui32 counter = list_size; counter != position; counter-- )
            listArray[ counter ] = listArray[ counter - 1 ];

        // Don't call constructors, assignment operators, etc.
        //memmove(listArray+position+1, listArray+position, (list_size-position)*sizeof(list_type));

        // Insert the new item at the correct spot
        listArray[ position ] = input;

        ++list_size;

    }


    template <class list_type>
    void List<list_type>::Insert(const list_type & input)
    {
        // Reallocate list if necessary

        if ( list_size == allocation_size )
        {
            // allocate twice the currently allocated memory
            list_type * new_array;

            if ( allocation_size == 0 )
                allocation_size = 16;
            else
                allocation_size *= 2;

            new_array = N_alloc_t(list_type, allocation_size);

            if (listArray)
            {
                // copy old array over
                    for ( Nui32 counter = 0; counter < list_size; ++counter )
                        new_array[ counter ] = listArray[ counter ];

                // Don't call constructors, assignment operators, etc.
                //memcpy(new_array, listArray, list_size*sizeof(list_type));

                // set old array to point to the newly allocated and twice as large array
                N_free(listArray);
            }

            listArray = new_array;
        }

        // Insert the new item at the correct spot
        listArray[ list_size ] = input;

        ++list_size;
    }

    template <class list_type>
        inline void List<list_type>::Replace(const list_type & input, const list_type filler, const Nui32 position)
    {
        if ( ( list_size > 0 ) && ( position < list_size ) )
        {
            // Direct replacement
            listArray[ position ] = input;
        }
        else
        {
            if ( position >= allocation_size )
            {
                // Reallocate the list to size position and fill in blanks with filler
                list_type * new_array;
                allocation_size = position + 1;

                new_array = N_alloc_t(list_type, allocation_size);

                // copy old array over

                for ( Nui32 counter = 0; counter < list_size; ++counter )
                    new_array[ counter ] = listArray[ counter ];

                // Don't call constructors, assignment operators, etc.
                //memcpy(new_array, listArray, list_size*sizeof(list_type));

                // set old array to point to the newly allocated array
                N_free(listArray);

                listArray = new_array;
            }

            // Fill in holes with filler
            while ( list_size < position )
                listArray[ list_size++ ] = filler;

            // Fill in the last element with the new item
            listArray[ list_size++ ] = input;

#ifdef _DEBUG

            N_assert( list_size == position + 1 );

#endif

        }
    }

    template <class list_type>
        inline void List<list_type>::Replace( const list_type &input )
    {
        if ( list_size > 0 )
            listArray[ list_size - 1 ] = input;
    }

    template <class list_type>
        void List<list_type>::RemoveAtIndex(const Nui32 position)
    {
#ifdef _DEBUG
        if(position >= list_size)
        {
            N_assert(position < list_size);
            return;
        }
#endif

        if(position < list_size)
        {
            // Compress the array
            for(Nui32 counter = position; counter < list_size - 1 ; ++counter)
            listArray[counter] = listArray[counter + 1];
            // Don't call constructors, assignment operators, etc.
            // memmove(listArray+position, listArray+position+1, (list_size-1-position) * sizeof(list_type));

            RemoveFromEnd();
        }
    }

    template <class list_type>
        void List<list_type>::RemoveAtIndexFast(const Nui32 position)
        {
#ifdef _DEBUG
            if (position >= list_size)
            {
                N_assert( position < list_size );
                return;
            }
#endif
            --list_size;
            listArray[position] = listArray[list_size];
        }

    template <class list_type>
        inline void List<list_type>::RemoveFromEnd(const Nui32 num)
    {
        // Delete the last elements on the list.  No compression needed
#ifdef _DEBUG
        N_assert(list_size >= num);
#endif
        list_size -= num;
    }

    template <class list_type>
        Nui32 List<list_type>::GetIndexOf(const list_type & input) const
    {
        for ( Nui32 i = 0; i < list_size; ++i )
            if ( listArray[ i ] == input )
                return i;

        return MAX_UNSIGNED_LONG;
    }

    template <class list_type>
        inline Nui32 List<list_type>::Size( void ) const
    {
        return list_size;
    }

    template <class list_type>
    void List<list_type>::Clear(bool doNotDeallocateSmallBlocks)
    {
        if ( allocation_size == 0 )
            return;

        if (allocation_size>512 || doNotDeallocateSmallBlocks==false)
        {
            N_free(listArray);
            allocation_size = 0;
            listArray = 0;
        }
        list_size = 0;
    }

    template <class list_type>
    void List<list_type>::Compress()
    {
        list_type * new_array;

        if ( allocation_size == 0 )
            return ;

        new_array = N_alloc_t(list_type, allocation_size);

        // copy old array over
        for ( Nui32 counter = 0; counter < list_size; ++counter )
            new_array[ counter ] = listArray[ counter ];

        // Don't call constructors, assignment operators, etc.
        //memcpy(new_array, listArray, list_size*sizeof(list_type));

        // set old array to point to the newly allocated array
        N_free(listArray);

        listArray = new_array;
    }

    template <class list_type>
    void List<list_type>::Preallocate(Nui32 countNeeded)
    {
        Nui32 amountToAllocate = allocation_size;
        if (allocation_size == 0)
            amountToAllocate = 16;
        while (amountToAllocate < countNeeded)
            amountToAllocate <<= 1;

        if ( allocation_size < amountToAllocate)
        {
            // allocate twice the currently allocated memory
            list_type * new_array;

            allocation_size=amountToAllocate;

            new_array = N_alloc_t(list_type, allocation_size);

            if (listArray)
            {
                // copy old array over
                for ( Nui32 counter = 0; counter < list_size; ++counter )
                    new_array[ counter ] = listArray[ counter ];

                // Don't call constructors, assignment operators, etc.
                //memcpy(new_array, listArray, list_size*sizeof(list_type));

                // set old array to point to the newly allocated and twice as large array
                N_free(listArray);
            }
            listArray = new_array;
        }
    }
} // End namespace
#endif