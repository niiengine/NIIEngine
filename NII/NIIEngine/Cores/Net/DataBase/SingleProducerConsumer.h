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

#ifndef __SINGLE_PRODUCER_CONSUMER_H
#define __SINGLE_PRODUCER_CONSUMER_H

#include "NiiNetPreInclude.h"

static const int MINIMUM_LIST_SIZE=8;

/// The namespace DataStructures was only added to avoid compiler errors for commonly named data structures
/// As these data structures are stand-alone, you can use them outside of RakNet for your own projects if you wish.
namespace DataStructures
{
	/// \brief A single producer consumer implementation without critical sections.
	template <class SingleProducerConsumerType>
	class _EngineAPI SingleProducerConsumer
	{
	public:
		// Constructor
		SingleProducerConsumer();

		// Destructor
		~SingleProducerConsumer();

		/// WriteLock must be immediately followed by WriteUnlock.  These two functions must be called in the same thread.
		/// \return A pointer to a block of data you can write to.
		SingleProducerConsumerType* WriteLock(void);

		/// Call if you don't want to write to a block of data from WriteLock() after all.
		/// Cancelling locks cancels all locks back up to the data passed.  So if you lock twice and cancel using the first lock, the second lock is ignored
		/// \param[in] cancelToLocation Which WriteLock() to cancel.
		void CancelWriteLock(SingleProducerConsumerType* cancelToLocation);

		/// Call when you are done writing to a block of memory returned by WriteLock()
		void WriteUnlock(void);

		/// ReadLock must be immediately followed by ReadUnlock. These two functions must be called in the same thread.
		/// \retval 0 No data is availble to read
		/// \retval Non-zero The data previously written to, in another thread, by WriteLock followed by WriteUnlock.
		SingleProducerConsumerType* ReadLock(void);

		// Cancelling locks cancels all locks back up to the data passed.  So if you lock twice and cancel using the first lock, the second lock is ignored
		/// param[in] Which ReadLock() to cancel.
		void CancelReadLock(SingleProducerConsumerType* cancelToLocation);

		/// Signals that we are done reading the the data from the least recent call of ReadLock.
		/// At this point that pointer is no longer valid, and should no longer be read.
		void ReadUnlock(void);

		/// Clear is not thread-safe and none of the lock or unlock functions should be called while it is running.
		void Clear(void);

		/// This function will estimate how many elements are waiting to be read.  It's threadsafe enough that the value returned is stable, but not threadsafe enough to give accurate results.
		/// \return An ESTIMATE of how many data elements are waiting to be read
		int Size(void) const;

		/// Make sure that the pointer we done reading for the call to ReadUnlock is the right pointer.
		/// param[in] A previous pointer returned by ReadLock()
		bool CheckReadUnlockOrder(const SingleProducerConsumerType* data) const;

		/// Returns if ReadUnlock was called before ReadLock
		/// \return If the read is locked
		bool ReadIsLocked(void) const;

	private:
		struct DataPlusPtr
		{
			DataPlusPtr () {readyToRead=false;}
			SingleProducerConsumerType object;

			// Ready to read is so we can use an equality boolean comparison, in case the writePointer var is trashed while context switching.
			volatile bool readyToRead;
			volatile DataPlusPtr *next;
		};
		volatile DataPlusPtr *readAheadPointer;
		volatile DataPlusPtr *writeAheadPointer;
		volatile DataPlusPtr *readPointer;
		volatile DataPlusPtr *writePointer;
		Nui32 readCount, writeCount;
	};

	template <class SingleProducerConsumerType>
		SingleProducerConsumer<SingleProducerConsumerType>::SingleProducerConsumer()
	{
		// Preallocate
		readPointer = N_new DataPlusPtr();
		writePointer=readPointer;
		readPointer->next = N_new DataPlusPtr();
		int listSize;
#ifdef _DEBUG
		N_assert(MINIMUM_LIST_SIZE>=3);
#endif
		for (listSize=2; listSize < MINIMUM_LIST_SIZE; listSize++)
		{
			readPointer=readPointer->next;
			readPointer->next = N_new DataPlusPtr();
		}
		readPointer->next->next=writePointer; // last to next = start
		readPointer=writePointer;
		readAheadPointer=readPointer;
		writeAheadPointer=writePointer;
		readCount=writeCount=0;
	}

	template <class SingleProducerConsumerType>
		SingleProducerConsumer<SingleProducerConsumerType>::~SingleProducerConsumer()
	{
		volatile DataPlusPtr *next;
		readPointer=writeAheadPointer->next;
		while (readPointer!=writeAheadPointer)
		{
			next=readPointer->next;
			N_delete readPointer;
			readPointer=next;
		}
		N_delete readPointer;
	}

	template <class SingleProducerConsumerType>
		SingleProducerConsumerType* SingleProducerConsumer<SingleProducerConsumerType>::WriteLock( void )
	{
		if (writeAheadPointer->next==readPointer ||
			writeAheadPointer->next->readyToRead==true)
		{
			volatile DataPlusPtr *originalNext=writeAheadPointer->next;
			writeAheadPointer->next = N_new DataPlusPtr();
			N_assert(writeAheadPointer->next);
			writeAheadPointer->next->next=originalNext;
		}

		volatile DataPlusPtr *last;
		last=writeAheadPointer;
		writeAheadPointer=writeAheadPointer->next;

		return (SingleProducerConsumerType*) last;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::CancelWriteLock( SingleProducerConsumerType* cancelToLocation )
	{
		writeAheadPointer=(DataPlusPtr *)cancelToLocation;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::WriteUnlock( void )
	{
		//	DataPlusPtr *dataContainer = (DataPlusPtr *)structure;

#ifdef _DEBUG
		N_assert(writePointer->next!=readPointer);
		N_assert(writePointer!=writeAheadPointer);
#endif

		writeCount++;
		// User is done with the data, allow send by updating the write pointer
		writePointer->readyToRead=true;
		writePointer=writePointer->next;
	}

	template <class SingleProducerConsumerType>
		SingleProducerConsumerType* SingleProducerConsumer<SingleProducerConsumerType>::ReadLock( void )
	{
			if (readAheadPointer==writePointer ||
				readAheadPointer->readyToRead==false)
			{
				return 0;
			}

			volatile DataPlusPtr *last;
			last=readAheadPointer;
			readAheadPointer=readAheadPointer->next;
			return (SingleProducerConsumerType*)last;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::CancelReadLock( SingleProducerConsumerType* cancelToLocation )
	{
#ifdef _DEBUG
		N_assert(readPointer!=writePointer);
#endif
		readAheadPointer=(DataPlusPtr *)cancelToLocation;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::ReadUnlock( void )
	{
#ifdef _DEBUG
		N_assert(readAheadPointer!=readPointer); // If hits, then called ReadUnlock before ReadLock
		N_assert(readPointer!=writePointer); // If hits, then called ReadUnlock when Read returns 0
#endif
		readCount++;

		// Allow writes to this memory block
		readPointer->readyToRead=false;
		readPointer=readPointer->next;
	}

	template <class SingleProducerConsumerType>
		void SingleProducerConsumer<SingleProducerConsumerType>::Clear( void )
	{
		// Shrink the list down to MINIMUM_LIST_SIZE elements
		volatile DataPlusPtr *next;
		writePointer=readPointer->next;

		int listSize=1;
		next=readPointer->next;
		while (next!=readPointer)
		{
			listSize++;
			next=next->next;
		}

		while (listSize-- > MINIMUM_LIST_SIZE)
		{
			next=writePointer->next;
#ifdef _DEBUG
			N_assert(writePointer!=readPointer);
#endif
			N_delete writePointer;
			writePointer=next;
		}

		readPointer->next=writePointer;
		writePointer=readPointer;
		readAheadPointer=readPointer;
		writeAheadPointer=writePointer;
		readCount=writeCount=0;
	}

	template <class SingleProducerConsumerType>
		int SingleProducerConsumer<SingleProducerConsumerType>::Size( void ) const
	{
		return writeCount-readCount;
	}

	template <class SingleProducerConsumerType>
		bool SingleProducerConsumer<SingleProducerConsumerType>::CheckReadUnlockOrder(const SingleProducerConsumerType* data) const
	{
		return const_cast<const SingleProducerConsumerType *>(&readPointer->object) == data;
	}


	template <class SingleProducerConsumerType>
		bool SingleProducerConsumer<SingleProducerConsumerType>::ReadIsLocked(void) const
	{
		return readAheadPointer!=readPointer;
	}	
}

#endif
