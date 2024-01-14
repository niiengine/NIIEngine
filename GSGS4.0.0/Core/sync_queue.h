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
 #ifndef SYNC_QUEUE_H
#define SYNC_QUEUE_H

#include "gsgsPreInclude.h"
#include "wx/thread.h"

/**
 * A thread safe queue implementation
 */
template <class T> _gsgsAPI class SyncQueue {
	std::deque<T> m_queue;
	wxCriticalSection m_cs;
public:
	SyncQueue(){}

	virtual ~SyncQueue(){
		wxCriticalSectionLocker lk(m_cs);
		m_queue.clear();
	}

	/**
	 * Add element to the top of the queue
	 */
	void PushFront(T element) {
		wxCriticalSectionLocker lk(m_cs);
		m_queue.push_front(element);
	}

	/**
	 * pop element from the top of the queue
	 */
	T PopFront(){
		wxCriticalSectionLocker lk(m_cs);
		if( m_queue.empty() ){
			return NULL;
		}
		T e = m_queue.front();
		m_queue.pop_front();
		return e;
	}

	/**
	 * Add element to the end of the queue
	 */
	void PushBack(T element) {
		wxCriticalSectionLocker lk(m_cs);
		m_queue.push_back(element);
	}

	/**
	 * pop element from the end of the queue
	 */
	T PopBack(){
		wxCriticalSectionLocker lk(m_cs);
		if( m_queue.empty() ){
			return NULL;
		}
		T e = m_queue.back();
		m_queue.pop_back();
		return e;
	}

	/**
	 * Return number of elements on the queue
	 */
	size_t GetCount() {
		wxCriticalSectionLocker lk(m_cs);
		return m_queue.size();
	}

	/**
	 * Clear the queue - remove all elements from the queue
	 */
	void Clear()  {
		wxCriticalSectionLocker lk(m_cs);
		return m_queue.clear();
	}
};
#endif // SYNC_QUEUE_H
