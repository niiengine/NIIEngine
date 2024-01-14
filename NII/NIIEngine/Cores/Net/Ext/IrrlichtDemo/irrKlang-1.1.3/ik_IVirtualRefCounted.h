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

#ifndef __I_IRRKLANG_VIRTUAL_UNKNOWN_H_INCLUDED__
#define __I_IRRKLANG_VIRTUAL_UNKNOWN_H_INCLUDED__

#include "ik_irrKlangTypes.h"


namespace irrklang
{

	//! Reference counting base class for objects in the Irrlicht Engine similar to IRefCounted.
	/** See IRefCounted for the basics of this class.
	The difference to IRefCounted is that the class has to implement reference counting
	for itself. 
	*/
	class IVirtualRefCounted
	{
	public:

		//! Destructor.
		virtual ~IVirtualRefCounted()
		{
		}

		//! Grabs the object. Increments the reference counter by one.
		/** To be implemented by the derived class. If you don't want to
		implement this, use the class IRefCounted instead. See IRefCounted::grab() for details
		of this method. */
		virtual void grab() = 0;

		//! Drops the object. Decrements the reference counter by one.
		/** To be implemented by the derived class. If you don't want to
		implement this, use the class IRefCounted instead. See IRefCounted::grab() for details
		of this method. */
		virtual bool drop() = 0;
	};



} // end namespace irrklang



#endif

