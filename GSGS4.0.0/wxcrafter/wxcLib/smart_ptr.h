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
 #ifndef CODELITE_SMART_PTR_H
#define CODELITE_SMART_PTR_H

/**
 * A smart pointer class that provides a reference counting and auto delete memory.
 *
 * This class is similar to std::auto_ptr, with 2 exceptions:
 * - This class uses reference counting
 * - We dont provide a release() function (because of the reference counting)
 * It is recommended to use this class instead of using raw pointer wherever possible.
 *
 * \note smart pointer to NULL is valid.
 *
 * \ingroup CodeLite
 * \version 1.0
 * first version
 * \date 09-17-2006
 * \author Eran
 */
template <class T>
class SharedPtr
{
	/**
     * The reference counting class
     *
	 * \ingroup CodeLite
	 * \version 1.0
	 * first version
	 *
	 * \date 09-17-2006
	 * \author Eran
	 */
	class SmartPtrRef
	{
		T* m_data;
		int m_refCount;

	public:
		/**
		 * Construct a reference counting class for row pointer data
		 * \param data pointer
		 */
		SmartPtrRef(T* data)
			: m_data( data )
			, m_refCount( 1 )
		{
		}

		/**
		 * Destructor
		 */
		virtual ~SmartPtrRef()
		{
			delete m_data;
		}

		/**
		 * \return Pointer to the row data 
		 */
		T* GetData() { return m_data; }
        
        const T* GetData() const { return m_data; }
		/**
		 * Increase reference counting by 1
		 */
		void IncRef() { m_refCount ++ ; }


		/**
		 * Decrease reference counting by 1
		 */
		void DecRef() { m_refCount -- ; }
		/**
		 * Return the current reference counting
		 * \return current reference counting
		 */
		int  GetRefCount() { return m_refCount; }
	};

	SmartPtrRef *m_ref;

public:
	/**
	 * Construct smart pointer from ptr
	 * \param ptr pointer
	 */
	SharedPtr(T* ptr)
	{
		// create a fresh copy
		CreateFresh( ptr );
	}
	
	/**
	 * Default constructor
	 */
	SharedPtr()
		: m_ref(NULL)
	{
	}

	/**
	 * Copy constructor
	 * \param rhs right hand side 
	 */
	SharedPtr(const SharedPtr& rhs)
		: m_ref(NULL)
	{
		*this = rhs;
	}

	/**
	 * Assignment operator
	 * \param rhs right hand side 
	 * \return reference to this
	 */
	SharedPtr& operator=(const SharedPtr& rhs)
	{
		// increase the reference count
		if( m_ref == rhs.m_ref )
			return *this;

		// Delete previous reference 
		DeleteRefCount();

		if( !rhs.m_ref )
			return *this;

		m_ref = rhs.m_ref;
		if (m_ref) {
			m_ref->IncRef();
		}
		return *this;
	}

	/**
	 * Destructor
	 */
	virtual ~SharedPtr()
	{
		DeleteRefCount();
	}

	/**
	 * Replace the current pointer with ptr
	 * if the current ptr is not NULL, it will be freed (reference counting free) before assingning the new ptr
	 * \param ptr new pointer
	 */
	void reset(T* ptr)
	{	
		DeleteRefCount();
		CreateFresh( ptr );
	}

	/**
	 * Return pointer the row data pointer
	 * \return pointer to the row data pointer
	 */
	T* get()
	{
		return m_ref->GetData();
	}
    
    const T* get() const 
    {
        return m_ref->GetData();
    }
	/**
	 * Overload the '->' operator 
	 * \return pointer to the row data pointer
	 */
	T* operator->() const 
	{
		return m_ref->GetData();
	}

	/**
	 * Dereference operator
	 * \return dereference the row data
	 */
	T& operator*() const
	{
		return *(m_ref->GetData());
	}

	/**
	 * Test for NULL operator
	 * \return true if the internal row data or the reference counting class are NULL false otherwise
	 */
	bool operator!() const
	{
		if( !m_ref )
			return true;

		return m_ref->GetData() == NULL;
	}

	/**
	 * test for bool operation
	 * \return true of the internal raw data exist and it is not null
	 */
	operator bool() const
	{
		return m_ref && m_ref->GetData();
	}

private:
	void DeleteRefCount()
	{
		// decrease the ref count (or delete pointer if it is 1)
		if( m_ref )
		{
			if( m_ref->GetRefCount() == 1 )
			{
				delete m_ref;
				m_ref = NULL;
			}
			else
				m_ref->DecRef();
		}
	};

	void CreateFresh(T* ptr)
	{
		m_ref = new SmartPtrRef( ptr );
	}
};


#endif // CODELITE_SMART_PTR_H
